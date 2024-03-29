#include <DawnStar/dspch.hpp>
#include <DawnStar/Renderer/Renderer2D.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <DawnStar/Renderer/Shader.hpp>
#include <DawnStar/Renderer/VertexArray.hpp>
#include <DawnStar/Renderer/RenderCommand.hpp>
#include <DawnStar/Renderer/Texture.hpp>
#include "Renderer2D.hpp"


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
    };

	static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        s_Data.QuadVertexArray = CreateRef<VertexArray>();

        s_Data.QuadVertexBuffer = CreateRef<VertexBuffer>(Renderer2DData::MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" },
        });
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

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
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

        // Lines
		{
			s_Data.LineVertexArray = CreateRef<VertexArray>();
			s_Data.LineVertexBuffer = CreateRef<VertexBuffer>(Renderer2DData::MaxVertices * sizeof(LineVertex));
			s_Data.LineVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" }
			});
			s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
			s_Data.LineVertexBufferBase = new LineVertex[Renderer2DData::MaxVertices];
		}
		
		// Text
		s_Data.TextVertexArray = CreateRef<VertexArray>();

		s_Data.TextVertexBuffer = CreateRef<VertexBuffer>(s_Data.MaxVertices * sizeof(TextVertex));
		s_Data.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     }
		});
		s_Data.TextVertexArray->AddVertexBuffer(s_Data.TextVertexBuffer);
		s_Data.TextVertexArray->SetIndexBuffer(quadIB);
		s_Data.TextVertexBufferBase = new TextVertex[s_Data.MaxVertices];

		s_Data.WhiteTexture = CreateRef<Texture2D>(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[32];
		for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
			samplers[i] = static_cast<int32_t>(i);
		
		s_Data.LineShader = CreateRef<Shader>("assets/shaders/Renderer2D_Line.glsl");
		s_Data.TextShader = CreateRef<Shader>("assets/shaders/Renderer2D_Text.glsl");
		s_Data.TextureShader = CreateRef<Shader>("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);

		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
    }

    void Renderer2D::Shutdown()
    {
        delete[] s_Data.QuadVertexBufferBase;
        delete[] s_Data.LineVertexBufferBase;
    }

    void Renderer2D::BeginScene(const glm::mat4& viewProjection)
    {
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", viewProjection);

		s_Data.LineShader->Bind();
		s_Data.LineShader->SetMat4("u_ViewProjection", viewProjection);
		
		s_Data.TextShader->Bind();
		s_Data.TextShader->SetMat4("u_ViewProjection", viewProjection);

		StartBatch();
    }

	void Renderer2D::EndScene()// const Ref<RenderGraphData>& renderGraphData)
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

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
		
		s_Data.TextIndexCount = 0;
		s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
        
        if(s_Data.QuadIndexCount > 0)
        {
            const auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) - reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferBase));
            s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

            for(uint32_t i = 0; i < s_Data.TextureSlotIndex;i++)
                s_Data.TextureSlots[i]->Bind(i);
			s_Data.TextureShader->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
        }

		if (s_Data.LineVertexCount > 0)
		{
			const auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(s_Data.LineVertexBufferPtr) - reinterpret_cast<uint8_t*>(s_Data.LineVertexBufferBase));
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);
			s_Data.LineShader->Bind();
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
			s_Data.Stats.DrawCalls++;
		}
		
		if (s_Data.TextIndexCount > 0)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.TextVertexBufferPtr - (uint8_t*)s_Data.TextVertexBufferBase);
			s_Data.TextVertexBuffer->SetData(s_Data.TextVertexBufferBase, dataSize);
			s_Data.FontAtlasTexture->Bind(0);

			s_Data.TextShader->Bind();
			RenderCommand::DrawIndexed(s_Data.TextVertexArray, s_Data.TextIndexCount);
			s_Data.Stats.DrawCalls++;
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
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if(*s_Data.TextureSlots[i] == *texture)
				{
					textureIndex = static_cast<float>(i);
					break;
				}
			}

			if(textureIndex == 0.0f)
			{
				if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
					NextBatch();
				
				textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
				s_Data.TextureSlotIndex++;
			}
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * Renderer2DData::QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}


	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		DS_PROFILE_SCOPE()
		
		constexpr size_t quadVertexCount = 4;
		constexpr float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		constexpr float tilingFactor = 1.0f;
		
		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * Renderer2DData::QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}
    
	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color)
	{
		DS_PROFILE_SCOPE()

		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexCount += 2;
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
		s_Data.FontAtlasTexture = font->GetAtlasTexture();

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
					// TODO: 실제 글자의 높이를 알려고 헀는데, 아래 코드로는 lineHeight와 동일한 결과를 뱉음
					// 	     실제 글자의 높이 찾기
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
		y = -firstLineHeight* fsScale ;
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

			float texelWidth = 1.0f / s_Data.FontAtlasTexture->GetWidth();
			float texelHeight = 1.0f / s_Data.FontAtlasTexture->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.01f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = texCoordMin;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.01f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.01f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = texCoordMax;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.01f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			s_Data.TextVertexBufferPtr++;

			s_Data.TextIndexCount += 6;
			s_Data.Stats.QuadCount++;

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

		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		DS_PROFILE_SCOPE()

		return s_Data.Stats;
	}

} // namespace DawnStar
