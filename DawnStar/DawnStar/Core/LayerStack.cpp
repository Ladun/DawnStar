
#include <dspch.hpp>
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
		DS_PROFILE_SCOPE()

		_layers.emplace(_layers.begin() + _layerInsertIndex, layer);
		_layerInsertIndex++;
		layer->OnAttach();
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		DS_PROFILE_SCOPE()

		_layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		DS_PROFILE_SCOPE()
		
		auto it = std::find(_layers.begin(), _layers.begin() + _layerInsertIndex, layer);
		if (it != _layers.end())
		{
			layer->OnDetach();
			_layers.erase(it);
			_layerInsertIndex--;
		}
	}
	void LayerStack::PopOverlay(Layer* overlay)
	{
		DS_PROFILE_SCOPE()

		auto it = std::find(_layers.begin() + _layerInsertIndex, _layers.end(), overlay);
		if (it != _layers.end())
		{
			overlay->OnDetach();
			_layers.erase(it);
		}
	}
} // namespace DawnStar
