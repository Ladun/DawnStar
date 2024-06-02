#include <dspch.hpp>
#include <DawnStar/Renderer/Material.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <DawnStar/Renderer/Renderer3D.hpp>
#include <DawnStar/Renderer/Shader.hpp>
#include <DawnStar/Renderer/Texture.hpp>

namespace DawnStar
{
    
	Ref<Texture2D> Material::_whiteTexture;

    Material::Material(const std::filesystem::path& shaderPath)
    {
        DS_PROFILE_SCOPE()

        if(!_whiteTexture)
        {            
			_whiteTexture = CreateRef<Texture2D>(1, 1);
			uint32_t whiteTextureData = 0xffffffff;
			_whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
        }

        const std::string name = shaderPath.filename().string();

        if(!Renderer3D::GetShaderLibrary().Exists(name))
            _shader = Renderer3D::GetShaderLibrary().Load(shaderPath);
        else
            _shader = Renderer3D::GetShaderLibrary().Get(name);

        Invalidate();
    }

    Material::~Material()
    {
        DS_PROFILE_SCOPE()

        delete[] _buffer;
    }

    void Material::Invalidate()
    {
        DS_PROFILE_SCOPE()

        delete[] _buffer;

        _bufferSizeInBytes = 0;
        const auto& materialProperties = _shader->GetMaterialProperties();

		for (const auto& [_, property] : materialProperties)
			_bufferSizeInBytes += property.SizeInBytes;

        _buffer = new char[_bufferSizeInBytes];
        memset(_buffer, 0, _bufferSizeInBytes);

        uint32_t slot = 0;
        auto one = glm::vec4(1.0);
        for (auto& [name, property] : materialProperties)
        {            
			if (property.Type == MaterialPropertyType::Sampler2D)
			{
				memcpy(_buffer + property.OffsetInBytes, &slot, sizeof(uint32_t));
				_textures.emplace(slot, nullptr);
				slot++;
			}
			else if (property.Type == MaterialPropertyType::Float ||
				     property.Type == MaterialPropertyType::Float2 ||
				     property.Type == MaterialPropertyType::Float3 ||
				     property.Type == MaterialPropertyType::Float4)
			{
				if (name.find("albedo") != std::string::npos || name.find("Albedo") != std::string::npos)
					memcpy(_buffer + property.OffsetInBytes, glm::value_ptr(one), property.SizeInBytes);
				if (name.find("roughness") != std::string::npos || name.find("Roughness") != std::string::npos)
					memcpy(_buffer + property.OffsetInBytes, glm::value_ptr(one), property.SizeInBytes);
			}
        }            

    }

    void Material::Bind() const
    {
        DS_PROFILE_SCOPE()

        const auto& materialProperties = _shader->GetMaterialProperties();
        
        _shader->Bind();
        for (const auto& [name, property] : materialProperties)
        {
            char* bufferStart = _buffer + property.OffsetInBytes;
            uint32_t slot = *reinterpret_cast<uint32_t*>(bufferStart);
            switch(property.Type)
            {
                case MaterialPropertyType::None : break;
                case MaterialPropertyType::Sampler2D :
                {
                    _shader->SetInt(name, static_cast<int>(slot));
                    if(_textures.at(slot))
                        _textures.at(slot)->Bind(slot);
                    else
                        _whiteTexture->Bind(slot);
                    break;
                }
                case MaterialPropertyType::Bool:
                case MaterialPropertyType::Int :
                {
                    _shader->SetInt(name, *reinterpret_cast<int32_t*>(bufferStart));
                    break;
                }
                case MaterialPropertyType::Float:
                {
                    _shader->SetFloat(name, *reinterpret_cast<float*>(bufferStart));
                    break;
                }
                case MaterialPropertyType::Float2:
                {
                    _shader->SetFloat2(name, *reinterpret_cast<glm::vec2*>(bufferStart));
                    break;
                }
                case MaterialPropertyType::Float3:
                {
                    _shader->SetFloat3(name, *reinterpret_cast<glm::vec3*>(bufferStart));
                    break;
                }
                case MaterialPropertyType::Float4:
                {
                    _shader->SetFloat4(name, *reinterpret_cast<glm::vec4*>(bufferStart));
                    break;
                }
            }
        }
    }

    void Material::Unbind() const
    {
        DS_PROFILE_SCOPE()

        _shader->Unbind();
    }

    Ref<Shader> Material::GetShader() const
    {
        return _shader;
    }

    Ref<Texture2D> Material::GetTexture(uint32_t slot)
    {
        const auto it = _textures.find(slot);

        if (it != _textures.end())
            return it->second;
        
        return nullptr;
    }

    void Material::SetTexture(uint32_t slot, const Ref<Texture2D> &texture)
    {
        _textures[slot] = texture;
    }

    MaterialData Material::GetDataInternal(const std::string &name) const
    {
        DS_PROFILE_SCOPE()

        const auto& materialProperties = _shader->GetMaterialProperties();
		const auto& materialProperty = _shader->GetMaterialProperties().find(name);
		if (materialProperty != materialProperties.end())
			return _buffer + materialProperty->second.OffsetInBytes;

		return nullptr;
    }

    void Material::SetDataInternal(const std::string &name, MaterialData data) const
    {
		DS_PROFILE_SCOPE()

		const auto& materialProperties = _shader->GetMaterialProperties();
		const auto& property = _shader->GetMaterialProperties().find(name);
		if (property != materialProperties.end())
			memcpy(_buffer + property->second.OffsetInBytes, data, property->second.SizeInBytes);
    }

} // namespace DawnStar
