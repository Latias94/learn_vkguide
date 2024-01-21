#pragma once

#include "camera.h"
#include "vk_descriptors.h"
#include "vk_loader.h"
#include "vk_types.h"

struct GPUSceneData
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection;   // w for sun power
    glm::vec4 sunlightColor;
};

struct ComputePushConstants
{
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;
    glm::vec4 data4;
};

struct ComputeEffect
{
    const char* name;

    VkPipeline       pipeline;
    VkPipelineLayout layout;

    ComputePushConstants data;
};

struct RenderObject
{
    uint32_t indexCount;
    uint32_t firstIndex;
    VkBuffer indexBuffer;

    MaterialInstance* material;

    glm::mat4       transform;
    VkDeviceAddress vertexBufferAddress;
};

struct DrawContext
{
    std::vector<RenderObject> OpaqueSurfaces;
    std::vector<RenderObject> TransparentSurfaces;
};

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) { deletors.push_back(function); }

    void flush()
    {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)();   // call functors
        }

        deletors.clear();
    }
};

struct FrameData
{
    VkCommandPool   _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    VkSemaphore _swapchainSemaphore, _renderSemaphore;
    VkFence     _renderFence;

    DeletionQueue               _deletionQueue;
    DescriptorAllocatorGrowable _frameDescriptors;
};

struct GLTFMetallic_Roughness
{
    MaterialPipeline opaquePipeline;
    MaterialPipeline transparentPipeline;

    VkDescriptorSetLayout materialLayout;

    struct MaterialConstants
    {
        glm::vec4 colorFactors;
        // metallic and roughness parameters on r and b components
        glm::vec4 metal_rough_factors;
        // padding, we need it anyway for uniform buffers
        glm::vec4 extra[14];
    };

    struct MaterialResources
    {
        AllocatedImage colorImage;
        VkSampler      colorSampler;
        AllocatedImage metalRoughImage;
        VkSampler      metalRoughSampler;
        VkBuffer       dataBuffer;
        uint32_t       dataBufferOffset;
    };

    DescriptorWriter writer;

    void build_pipelines(VulkanEngine* engine);
    void clear_resources(VkDevice device);

    MaterialInstance write_material(VkDevice device, MaterialPass pass,
                                    const MaterialResources&     resources,
                                    DescriptorAllocatorGrowable& descriptorAllocator);
};

struct MeshNode : public Node
{
    std::shared_ptr<MeshAsset> mesh;

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
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

    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage,
                                  VmaMemoryUsage memoryUsage);
    void            destroy_buffer(const AllocatedBuffer& buffer);

    AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage,
                                bool mipmapped = false);
    AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage, bool mipmapped = false);
    void           destroy_image(const AllocatedImage& img);

private:
    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();
    void init_descriptors();
    void init_imgui();
    void init_default_data();

    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();
    void resize_swapchain();

    void draw_background(VkCommandBuffer cmd);
    void draw_geometry(VkCommandBuffer cmd);

    void init_pipelines();
    void init_background_pipelines();
    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

    void update_scene();

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

    VmaAllocator _allocator;

    AllocatedImage _drawImage;
    AllocatedImage _depthImage;
    VkExtent2D     _drawExtent;
    float          renderScale = 1.f;

    DescriptorAllocatorGrowable globalDescriptorAllocator;

    VkDescriptorSet       _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    VkPipelineLayout _gradientPipelineLayout;

    // immediate submit structures (ImGui)
    VkFence         _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool   _immCommandPool;

    GPUMeshBuffers rectangle;

    GPUSceneData sceneData;

    VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

    AllocatedImage _whiteImage;
    AllocatedImage _blackImage;
    AllocatedImage _greyImage;
    AllocatedImage _errorCheckerboardImage;

    VkSampler _defaultSamplerLinear;
    VkSampler _defaultSamplerNearest;

    VkDescriptorSetLayout _singleImageDescriptorLayout;

    MaterialInstance       defaultData;
    GLTFMetallic_Roughness metalRoughMaterial;

    DrawContext mainDrawContext;

    std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;

    bool freeze_rendering{false};
    bool resize_requested{false};

private:
    DeletionQueue _mainDeletionQueue;

    std::vector<ComputeEffect> backgroundEffects;
    int                        currentBackgroundEffect{0};

    std::vector<std::shared_ptr<MeshAsset>> testMeshes;

    Camera mainCamera;
};
