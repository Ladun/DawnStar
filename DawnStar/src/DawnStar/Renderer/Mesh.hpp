#pragma once

namespace DawnStar
{
    class VertexArray;
    class Material;

    struct Submesh
    {
        std::string Name;
        Ref<Material> Mat;
        Ref<VertexArray> Geometry;
        
		Submesh(const std::string& name, const Ref<Material>& material, const Ref<VertexArray>& geometry)
			: Name(name), Mat(material), Geometry(geometry)
		{
		}
    };

    class Mesh
    {
    public:
        Mesh() = default;
        explicit Mesh(const char* filepath);
        virtual ~Mesh() = default;

        void Load(const char* filepath);

        Submesh& GetSubmesh(size_t index);
        size_t GetSubmeshCount() const { return _submeshes.size(); }
		const char* GetName() const { return _name.c_str(); }
		const char* GetFilepath() const { return _filepath.c_str(); }

    private:
        std::string _name;
        std::string _filepath;
        std::vector<Submesh> _submeshes;
    };
} // namespace DawnStar
