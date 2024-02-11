
#include <DawnStar/dspch.hpp>
#include <DawnStar/Core/LayerStack.hpp>

namespace DawnStar
{
    LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{	
		for (Layer* layer : _layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}
    void LayerStack::PushLayer(Layer* layer)
	{
		_layers.emplace(_layers.begin() + _layerInsertIndex, layer);
		_layerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		_layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		// TODO: free memory that layer used;
		auto it = std::find(_layers.begin(), _layers.end(), layer);
		if (it != _layers.end())
		{
			layer->OnDetach();
			_layers.erase(it);
			_layerInsertIndex--;
		}
	}
	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(_layers.begin(), _layers.end(), overlay);
		if (it != _layers.end())
		{
			overlay->OnDetach();
			_layers.erase(it);
		}
	}
} // namespace DawnStar
