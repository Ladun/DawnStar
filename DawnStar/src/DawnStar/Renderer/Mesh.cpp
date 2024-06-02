#include <dspch.hpp>
#include <DawnStar/Renderer/Mesh.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <DawnStar/Core/AssetManager.hpp>
#include <DawnStar/Renderer/Material.hpp>
#include <DawnStar/Renderer/Shader.hpp>
#include <DawnStar/Renderer/VertexArray.hpp>
#include <DawnStar/Utils/StringUtils.hpp>


namespace DawnStar
{
	struct Vertex
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec2 TexCoord = glm::vec2(0.0f);
		glm::vec3 Normal = glm::vec3(0.0f);
		glm::vec3 Tangent = glm::vec3(0.0f);
		glm::vec3 Bitangent = glm::vec3(0.0f);

		bool operator==(const Vertex& other) const
		{
			return	Position == other.Position &&
				    TexCoord == other.TexCoord &&
				    Normal == other.Normal &&
				    Tangent == other.Tangent &&
				    Bitangent == other.Bitangent;
		}
	};
} // namespace DawnStar


namespace std
{
	template<> struct hash<DawnStar::Vertex>
	{
		size_t operator()(DawnStar::Vertex const& vertex) const noexcept
		{
			const std::size_t h1 = std::hash<glm::vec3>{}(vertex.Position);
			const std::size_t h2 = std::hash<glm::vec2>{}(vertex.TexCoord);
			const std::size_t h3 = std::hash<glm::vec3>{}(vertex.Normal);
			const std::size_t h4 = std::hash<glm::vec3>{}(vertex.Tangent);
			const std::size_t h5 = std::hash<glm::vec3>{}(vertex.Bitangent);
			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
		}
	};
}


namespace DawnStar
{

    Mesh::Mesh(const char *filepath)
    {
        DS_PROFILE_SCOPE()

        Load(filepath);
    }

    void Mesh::Load(const char *filepath)
    {
        DS_PROFILE_SCOPE()

        std::filesystem::path path = filepath;

        if(!std::filesystem::exists(path))
            return;

        auto ext = path.extension();
        bool supportedFile = ext == ".obj";
        if(!supportedFile)
        {
			DS_CORE_ERROR("{1} file(s) not supported: {0}", ext.string(), filepath);
			return;
        }

        if(ext == ".obj")
        {
            tinyobj::ObjReaderConfig reader_config;
			tinyobj::ObjReader reader;

			if(!reader.ParseFromFile(filepath, reader_config))
			{
				if(!reader.Error().empty())
				{
					DS_CORE_ERROR("Could not import the file: {0}. Error: {1}", filepath, reader.Error());
					return;
				}
			}
			
			if (!reader.Warning().empty())
				DS_CORE_ERROR("File: {0}. Warning: {1}", filepath, reader.Warning());

			auto& attrib 	= reader.GetAttrib();
			auto& shapes 	= reader.GetShapes();
			auto& materials = reader.GetMaterials();

			// Loop over shapes
			for(const auto& shape : shapes)
			{
				std::vector<Vertex> vertices;
				std::vector<uint32_t> indices;
				std::unordered_map<Vertex, uint32_t> uniqueVertices{};
				
				// Loop over faces(polygon)
				int materialId = -1;
				size_t index_offset = 0;
				for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
				{
					auto fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);

					// Loop over vertices in the face.
					for(size_t v = 0; v < fv; v++)
					{
						// access to vertex
						tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

						Vertex vertex;
						vertex.Position.x = attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 0];
						vertex.Position.y = attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 1];
						vertex.Position.z = attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 2];
						if(idx.texcoord_index >= 0)
						{
							vertex.TexCoord.x = attrib.texcoords[2 * static_cast<size_t>(idx.texcoord_index) + 0];
							vertex.TexCoord.y = attrib.texcoords[2 * static_cast<size_t>(idx.texcoord_index) + 1];
						}
						if (idx.normal_index >= 0)
						{
							vertex.Normal.x = attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 0];
							vertex.Normal.y = attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 1];
							vertex.Normal.z = attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 2];
						}

						if (uniqueVertices.count(vertex) == 0)
						{
							uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
							vertices.push_back(vertex);
						}

						indices.push_back(uniqueVertices[vertex]);
					}
					index_offset += fv;
					
					// per-face material
					materialId = shape.mesh.material_ids[f];
				}

				Ref<VertexArray> vertexArray = CreateRef<VertexArray>();

				Ref<VertexBuffer> vertexBuffer = CreateRef<VertexBuffer>(reinterpret_cast<float*>(vertices.data()), (sizeof(Vertex) * vertices.size()));
				vertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float2, "a_TexCoord" },
					{ ShaderDataType::Float3, "a_Normal" },
					{ ShaderDataType::Float3, "a_Tangent" },
					{ ShaderDataType::Float3, "a_Bitangent" },
				});
				vertexArray->AddVertexBuffer(vertexBuffer);

				Ref<IndexBuffer> indexBuffer = CreateRef<IndexBuffer>(indices.data(), indices.size());
				vertexArray->SetIndexBuffer(indexBuffer);

				const Submesh& submesh = _submeshes.emplace_back(shape.name, CreateRef<Material>(), vertexArray);

				if(materialId >= 0)
				{
					const auto& material = materials.at(materialId);

					const auto& materialProperties = submesh.Mat->GetShader()->GetMaterialProperties();
					bool normalMapApplied = false;
					
					std::filesystem::path dir = path.parent_path();

					for (const auto& [name, property] : materialProperties)
					{
						if(property.Type == MaterialPropertyType::Sampler2D)
						{
							auto slot = submesh.Mat->GetData<uint32_t>(name);

							if(!material.diffuse_texname.empty() && 
							   (name.find("albedo") != std::string::npos || name.find("Albedo") != std::string::npos ||
							    name.find("diff") != std::string::npos || name.find("Diff") != std::string::npos))
							{
								std::string pathStr = (dir / material.diffuse_texname).string();
								submesh.Mat->SetTexture(slot, AssetManager::GetTexture2D(pathStr));
							}

							if (!material.normal_texname.empty() &&
								(name.find("norm") != std::string::npos || name.find("Norm") != std::string::npos ||
									name.find("height") != std::string::npos || name.find("Height") != std::string::npos))
							{
								std::string pathStr = (dir / material.normal_texname).string();
								submesh.Mat->SetTexture(slot, AssetManager::GetTexture2D(pathStr));
								normalMapApplied = true;
							}
							else if (!material.bump_texname.empty() &&
								(name.find("norm") != std::string::npos || name.find("Norm") != std::string::npos ||
									name.find("height") != std::string::npos || name.find("Height") != std::string::npos))
							{
								std::string pathStr = (dir / material.bump_texname).string();
								submesh.Mat->SetTexture(slot, AssetManager::GetTexture2D(pathStr));
								normalMapApplied = true;
							}

							if (!material.emissive_texname.empty() &&
								(name.find("emissi") != std::string::npos || name.find("Emissi") != std::string::npos))
							{
								std::string pathStr = (dir / material.emissive_texname).string();
								submesh.Mat->SetTexture(slot, AssetManager::GetTexture2D(pathStr));
							}
						}
						
						if (property.Type == MaterialPropertyType::Bool && normalMapApplied &&
							(name.find("norm") != std::string::npos || name.find("Norm") != std::string::npos ||
								name.find("height") != std::string::npos || name.find("Height") != std::string::npos))
						{
							submesh.Mat->SetData(name, 1);
						}
					}
				}


			}
        }

        
		_filepath = filepath;
		_name = StringUtils::GetName(filepath);

    }

    Submesh &Mesh::GetSubmesh(size_t index)
    {
		DS_PROFILE_SCOPE()

		DS_CORE_ASSERT(index < _submeshes.size(), "Submesh index out of bounds")

		return _submeshes[index];
    }

} // namespace DawnStar