#pragma once

#include "vk_types.h"

struct FrameData
{
    VkCommandPool   _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    VkSemaphore _swapchainSemaphore, _renderSemaphore;
    VkFence     _renderFence;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine
{
public:
    // initializes everything in the engine
    void init();

    // shuts down the engine
    void cleanup();

    // draw loop
    void draw();

    // run main loop
    void run();

private:
    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();

    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();

public:
    bool _isInitialized{false};
    int  _frameNumber{0};

    VkExtent2D _windowExtent{1700, 900};

    struct SDL_Window* _window{nullptr};

    FrameData _frames[FRAME_OVERLAP];

    FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

    VkQueue  _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    VkInstance               _instance;          // Vulkan library handle
    VkDebugUtilsMessengerEXT _debug_messenger;   // Vulkan debug output handle
    VkPhysicalDevice         _chosenGPU;         // GPU chosen as the default device
    VkDevice                 _device;            // Vulkan device for commands
    VkSurfaceKHR             _surface;           // Vulkan window surface

    VkSwapchainKHR _swapchain;
    VkFormat       _swapchainImageFormat;

    std::vector<VkImage>     _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    VkExtent2D               _swapchainExtent;

    bool stop_rendering{false};
};
