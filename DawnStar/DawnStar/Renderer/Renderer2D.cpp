#include <dspch.hpp>
#include <DawnStar/Renderer/Renderer2D.hpp>
#include <DawnStar/Renderer/Renderer3D.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <DawnStar/Renderer/Shader.hpp>
#include <DawnStar/Renderer/VertexArray.hpp>
#include <DawnStar/Renderer/RenderCommand.hpp>
#include <DawnStar/Renderer/RenderGraphData.hpp>
#include <DawnStar/Renderer/Texture.hpp>



namespace DawnStar
{
    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float TilingFactor;
    };

    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
    };
	
	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;

		// TODO: bg color for outline/bg
	};

    struct Renderer2DData
    {
		static constexpr uint32_t MaxQuads = 20000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;
		static constexpr uint32_t MaxIndices = MaxQuads * 6;
		static constexpr uint32_t MaxTextureSlots = 32;
		
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		Ref<VertexArray> TextVertexArray;
		Ref<VertexBuffer> TextVertexBuffer;
		Ref<Shader> TextShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		
		uint32_t TextIndexCount = 0;
		TextVertex* TextVertexBufferBase = nullptr;
		TextVertex* TextVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
		
		Ref<Texture2D> FontAtlasTexture;

		static constexpr glm::vec4 QuadVertexPositions[4] =	  { { -0.5f, -0.5f, 0.0f, 1.0f },
																{  0.5f, -0.5f, 0.0f, 1.0f },
																{  0.5f,  0.5f, 0.0f, 1.0f },
																{ -0.5f,  0.5f, 0.0f, 1.0f } };
		Renderer2D::Statistics Stats;
		Ref<UniformBuffer> CameraUniformBuffer;
    };

	static Renderer2DData _data;

    void Renderer2D::Init()
    {
		// TODO: 3D Rendering하고 중복 
		//        힌 곳에서만 할 수 있도록 수정..?
        _data.CameraUniformBuffer = CreateRef<UniformBuffer>();
		_data.CameraUniformBuffer->SetLayout({
			{ ShaderDataType::Mat4, "u_View" },
			{ ShaderDataType::Mat4, "u_Projection" },
			{ ShaderDataType::Mat4, "u_ViewProjection" },
			{ ShaderDataType::Float4, "u_CameraPosition" }
		}, 0);

        _data.QuadVertexArray = CreateRef<VertexArray>();

        _data.QuadVertexBuffer = CreateRef<VertexBuffer>(Renderer2DData::MaxVertices * sizeof(QuadVertex));
        _data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" },
        });
		_data.QuadVertexArray->AddVertexBuffer(_data.QuadVertexBuffer);

		_data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

        const auto quadIndices = new uint32_t[Renderer2DData::MaxIndices];
        uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

        Ref<IndexBuffer> quadIB = CreateRef<IndexBuffer>(quadIndices, Renderer2DData::MaxIndices);
		_data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

        // Lines
		{
			_data.LineVertexArray = CreateRef<VertexArray>();
			_data.LineVertexBuffer = CreateRef<VertexBuffer>(Renderer2DData::MaxVertices * sizeof(LineVertex));
			_data.LineVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" }
			});
			_data.LineVertexArray->AddVertexBuffer(_data.LineVertexBuffer);
			_data.LineVertexBufferBase = new LineVertex[Renderer2DData::MaxVertices];
		}
		
		// Text
		_data.TextVertexArray = CreateRef<VertexArray>();

		_data.TextVertexBuffer = CreateRef<VertexBuffer>(_data.MaxVertices * sizeof(TextVertex));
		_data.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     }
		});
		_data.TextVertexArray->AddVertexBuffer(_data.TextVertexBuffer);
		_data.TextVertexArray->SetIndexBuffer(quadIB);
		_data.TextVertexBufferBase = new TextVertex[_data.MaxVertices];

		_data.WhiteTexture = CreateRef<Texture2D>(1,1);
		uint32_t whiteTextureData = 0xffffffff;
		_data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[32];
		for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
			samplers[i] = static_cast<int32_t>(i);
		
		_data.LineShader = CreateRef<Shader>("assets/shaders/Renderer2D_Line.glsl");
		_data.LineShader->Bind();
		_data.LineShader->SetUniformBlock("Camera", 0);

		_data.TextShader = CreateRef<Shader>("assets/shaders/Renderer2D_Text.glsl");
		_data.TextShader->Bind();
		_data.TextShader->SetUniformBlock("Camera", 0);

		_data.TextureShader = CreateRef<Shader>("assets/shaders/Renderer2D_Quad.glsl");
		_data.TextureShader->Bind();
		_data.TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);
		_data.TextureShader->SetUniformBlock("Camera", 0);

		// Set first texture slot to 0
		_data.TextureSlots[0] = _data.WhiteTexture;
    }

    void Renderer2D::Shutdown()
    {
        delete[] _data.QuadVertexBufferBase;
        delete[] _data.LineVertexBufferBase;
    }

    void Renderer2D::BeginScene(const CameraData& cameraData)
    {
		_data.CameraUniformBuffer->Bind();
		_data.CameraUniformBuffer->SetData(&cameraData, 0, sizeof(CameraData));

		StartBatch();
    }

	void Renderer2D::EndScene(const Ref<RenderGraphData>& renderGraphData)
	{
		DS_PROFILE_SCOPE()
		
		// renderGraphData->CompositePassTarget->Bind();
		RenderCommand::DisableCulling();
		Flush();
		// renderGraphData->CompositePassTarget->Unbind();
		
	}

	void Renderer2D::StartBatch()
	{
		DS_PROFILE_SCOPE()

		RenderCommand::SetBlendState(true);

		_data.QuadIndexCount = 0;
		_data.QuadVertexBufferPtr = _data.QuadVertexBufferBase;

		_data.LineVertexCount = 0;
		_data.LineVertexBufferPtr = _data.LineVertexBufferBase;
		
		_data.TextIndexCount = 0;
		_data.TextVertexBufferPtr = _data.TextVertexBufferBase;

		_data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
        
        if(_data.QuadIndexCount > 0)
        {
            const auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(_data.QuadVertexBufferPtr) - reinterpret_cast<uint8_t*>(_data.QuadVertexBufferBase));
            _data.QuadVertexBuffer->SetData(_data.QuadVertexBufferBase, dataSize);

            for(uint32_t i = 0; i < _data.TextureSlotIndex;i++)
                _data.TextureSlots[i]->Bind(i);
			_data.TextureShader->Bind();
			RenderCommand::DrawIndexed(_data.QuadVertexArray, _data.QuadIndexCount);
			_data.Stats.DrawCalls++;
        }

		if (_data.LineVertexCount > 0)
		{
			const auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(_data.LineVertexBufferPtr) - reinterpret_cast<uint8_t*>(_data.LineVertexBufferBase));
			_data.LineVertexBuffer->SetData(_data.LineVertexBufferBase, dataSize);
			_data.LineShader->Bind();
			RenderCommand::DrawLines(_data.LineVertexArray, _data.LineVertexCount);
			_data.Stats.DrawCalls++;
		}
		
		if (_data.TextIndexCount > 0)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)_data.TextVertexBufferPtr - (uint8_t*)_data.TextVertexBufferBase);
			_data.TextVertexBuffer->SetData(_data.TextVertexBufferBase, dataSize);
			_data.FontAtlasTexture->Bind(0);

			_data.TextShader->Bind();
			RenderCommand::DrawIndexed(_data.TextVertexArray, _data.TextIndexCount);
			_data.Stats.DrawCalls++;
		}
		RenderCommand::SetBlendState(false);
	}

	void Renderer2D::NextBatch()
	{
		DS_PROFILE_SCOPE()

		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, rotation, size, texture, tintColor, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor)
	{
		DS_PROFILE_SCOPE()

		
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
								* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
								* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tintColor, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor)
	{
		DS_PROFILE_SCOPE()        
		
		float textureIndex = 0.0f;

		if(texture)
		{
			for (uint32_t i = 1; i < _data.TextureSlotIndex; i++)
			{
				if(*_data.TextureSlots[i] == *texture)
				{
					textureIndex = static_cast<float>(i);
					break;
				}
			}

			if(textureIndex == 0.0f)
			{
				if(_data.QuadIndexCount >= Renderer2DData::MaxIndices)
					NextBatch();
				
				textureIndex = static_cast<float>(_data.TextureSlotIndex);
				_data.TextureSlots[_data.TextureSlotIndex] = texture;
				_data.TextureSlotIndex++;
			}
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			_data.QuadVertexBufferPtr->Position = transform * Renderer2DData::QuadVertexPositions[i];
			_data.QuadVertexBufferPtr->Color = tintColor;
			_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			_data.QuadVertexBufferPtr->TexIndex = textureIndex;
			_data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			_data.QuadVertexBufferPtr++;
		}
		
		_data.QuadIndexCount += 6;

		_data.Stats.QuadCount++;
	}


	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		DS_PROFILE_SCOPE()
		
		constexpr size_t quadVertexCount = 4;
		constexpr float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		constexpr float tilingFactor = 1.0f;
		
		if(_data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			_data.QuadVertexBufferPtr->Position = transform * Renderer2DData::QuadVertexPositions[i];
			_data.QuadVertexBufferPtr->Color = color;
			_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			_data.QuadVertexBufferPtr->TexIndex = textureIndex;
			_data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			_data.QuadVertexBufferPtr++;
		}
		
		_data.QuadIndexCount += 6;

		_data.Stats.QuadCount++;
	}
    
	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color)
	{
		DS_PROFILE_SCOPE()

		_data.LineVertexBufferPtr->Position = p0;
		_data.LineVertexBufferPtr->Color = color;
		_data.LineVertexBufferPtr++;

		_data.LineVertexBufferPtr->Position = p1;
		_data.LineVertexBufferPtr->Color = color;
		_data.LineVertexBufferPtr++;

		_data.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		DS_PROFILE_SCOPE()

		const glm::vec3 p0 = { position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z };
		const glm::vec3 p1 = { position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z };
		const glm::vec3 p2 = { position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z };
		const glm::vec3 p3 = { position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z };

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color)
	{
		DS_PROFILE_SCOPE()

		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * Renderer2DData::QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

    void Renderer2D::DrawString(const std::string &string, Ref<Font> font, const glm::mat4 &transform, const glm::vec2 size, const TextParams &textParams)
    {
		DS_PROFILE_SCOPE()
		
		const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		_data.FontAtlasTexture = font->GetAtlasTexture();

		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY) * textParams.FontSize;
		double x = 0.0;
		double y = 0.0;

		const float spaceGlyphsAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		// Calcuate full width and height	
		#pragma region Calculate the full width and height of Text	
		std::vector<double> widthOfEachColumn; 
		double height = 0;
		double firstLineHeight = 0.0;
		bool isFirstLine = true;
		for(size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if(character == '\r')
				continue;

			if (character == '\n')
			{
				widthOfEachColumn.push_back(x);
				x = 0;
				y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
				if (isFirstLine)
				{
					isFirstLine = false;
				} 
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphsAdvance;
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance + textParams.Kerning;
			}
			else if (character == '\t')
			{
				x += 4.0f * (fsScale * spaceGlyphsAdvance + textParams.Kerning);
				continue;
			}
			else 
			{
				auto glyph = fontGeometry.getGlyph(character);
				if (!glyph)
					glyph = fontGeometry.getGlyph('?');
				if (!glyph)
					return;
				if(isFirstLine)
				{
					double pl, pb, pr, pt;
					glyph->getQuadPlaneBounds(pl, pb, pr, pt);
					if(firstLineHeight < pt - pb)
						firstLineHeight = pt - pb;
				}

				double advance = glyph->getAdvance();
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					fontGeometry.getAdvance(advance, character, nextCharacter);
				}

				x += fsScale * advance + textParams.Kerning;
			}
		}
		widthOfEachColumn.push_back(x);
		height = -y + firstLineHeight * fsScale ;

		#pragma endregion

		#pragma region Drawing text
		// Drawing text
		x = 0.0;
		y = -firstLineHeight * fsScale ;
		int lineIdx = 0;
		for(size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if(character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
				lineIdx++;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphsAdvance;
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance + textParams.Kerning;
				continue;
			}

			if (character == '\t')
			{
				x += 4.0f * (fsScale * spaceGlyphsAdvance + textParams.Kerning);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			// Calucate 
			glm::vec2 adjustment = glm::vec2(0);

			// horizontal alignment
			if(textParams.Align & 0b0000'0010) // center
			{
				adjustment.x = -widthOfEachColumn[lineIdx] / 2;
			}
			else if(textParams.Align & 0b0000'0100) // right
			{
				adjustment.x = size.x / 2 - widthOfEachColumn[lineIdx];
			}
			else if(textParams.Align & 0b0000'0001) // left
			{
				adjustment.x = -size.x / 2;
			}
			// vertical alignment
			if(textParams.Align & 0b0000'1000) // top
			{
				adjustment.y = size.y / 2;
			}
			else if(textParams.Align & 0b0010'0000) // bottom
			{
				adjustment.y = -size.y / 2 + height;
			}
			else if(textParams.Align & 0b0001'0000) // center
			{
				adjustment.y = height / 2;
			}
			
			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y) + adjustment;
			quadMax += glm::vec2(x, y) + adjustment;

			float texelWidth = 1.0f / _data.FontAtlasTexture->GetWidth();
			float texelHeight = 1.0f / _data.FontAtlasTexture->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			_data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.01f, 1.0f);
			_data.TextVertexBufferPtr->Color = textParams.Color;
			_data.TextVertexBufferPtr->TexCoord = texCoordMin;
			_data.TextVertexBufferPtr++;

			_data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.01f, 1.0f);
			_data.TextVertexBufferPtr->Color = textParams.Color;
			_data.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			_data.TextVertexBufferPtr++;

			_data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.01f, 1.0f);
			_data.TextVertexBufferPtr->Color = textParams.Color;
			_data.TextVertexBufferPtr->TexCoord = texCoordMax;
			_data.TextVertexBufferPtr++;

			_data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.01f, 1.0f);
			_data.TextVertexBufferPtr->Color = textParams.Color;
			_data.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			_data.TextVertexBufferPtr++;

			// TODO: Combining Text drawing into one quad
			_data.TextIndexCount += 6;
			_data.Stats.QuadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.Kerning;
			}
			
		}
		#pragma endregion
    }

    void Renderer2D::DrawString(const glm::mat4 &transform, const glm::vec2 size, const TextComponent &component)
    {
		DrawString(component.TextString, component.FontAsset, transform, size, {component.Color, component.Kerning, component.LineSpacing, component.FontSize, component.Align});
	}

	void Renderer2D::ResetStats()
	{
		DS_PROFILE_SCOPE()

		memset(&_data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		DS_PROFILE_SCOPE()

		return _data.Stats;
	}

} // namespace DawnStar
