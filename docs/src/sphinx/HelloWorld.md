# Hello World

## Creating a Window

LavaCake use glfw3 as a window manager.
glfw is wrapped in the Window class : 

to create a window use the following command :

`LavaCake::Framework::Window window(title, width, height);`

* title : the name of your app.
* width : the width of your app.
* height : the height of your app.

The windows should open itself after that

To know if your window is still alive call the function window.running().
this function return true when the window is alive, and false when the windows must be killed.

To refresh the Window call window.updateInput()

A typical code to do a render using the LavaCake window manager should look like this : 


```
#include "LavaCake/Window.h"
using namespace LavaCake::Framework;


int main() {

	Window window("My window", 500, 500);
        //Vulkan Initialisation
        

        //Renderering setup


	while (window.running()) {
	window.updateInput();

	//render loop

	}
}
```
## Vulkan initialisation

To initialise vulkan in LavaCake you need to get a reference to the Device singleton provided by LavaCake and initialise it.\
This singleton holds references to Physical and Logical Device, Queues, Surface and Instances.


```
Device* d = Device::getDevice();
d->initDevices(0, 1, window.m_windowParams);
```


Then we need initialise the get a reference to the Swapchain singleton and initialise it.\
This singleton holds the rendering parameter such as the output size, the color and depth format etc...\
This singleton will also provide the swapchain images requiered to draw on the screen.

```
SwapChain* s = SwapChain::getSwapChain();
s->init();
```

## Renderering setup


## Final code

```
#include "Framework/Framework.h"


using namespace LavaCake;
using namespace LavaCake::Geometry;
using namespace LavaCake::Framework;
using namespace LavaCake::Core;


int main() {

	Window w("LavaCake HelloWorld", 512, 512);

	Device* d = Device::getDevice();
	d->initDevices(0, 1, window.m_windowParams);
	SwapChain* s = SwapChain::getSwapChain();
	s->init();
	VkExtent2D size = s->size();
	Queue* queue = d->getGraphicQueue(0);
	PresentationQueue* presentQueue = d->getPresentQueue();
	CommandBuffer  commandBuffer;
	commandBuffer.addSemaphore();


	//We define the stride format we need for the mesh here 
	//each vertex is a 3D position followed by a RGB color
	vertexFormat format = vertexFormat({ POS3,COL3 });

	//we create a indexed triangle mesh with the desired format
	Mesh_t* triangle = new IndexedMesh<TRIANGLE>(format);

	//adding 3 vertices
	triangle->appendVertex({ -0.75f, 0.75f, 0.0f, 1.0f , 0.0f , 0.0f });
	triangle->appendVertex({ 0.75f,	0.75f , 0.0f, 0.0f , 1.0f	, 0.0f });
	triangle->appendVertex({ 0.0f , -0.75f, 0.0f, 0.0f , 0.0f	, 1.0f });


	// we link the 3 vertices to define a triangle
	triangle->appendIndex(0);
	triangle->appendIndex(1);
	triangle->appendIndex(2);


	//creating an allocating a vertex buffer
	VertexBuffer* triangle_vertex_buffer = new VertexBuffer({ triangle });
	triangle_vertex_buffer->allocate(queue, commandBuffer);

	//load the shaders into shader module	
	VertexShaderModule* vertexShader = new VertexShaderModule("Data/Shaders/helloworld/shader.vert.spv");
	FragmentShaderModule* fragmentShader = new FragmentShaderModule("Data/Shaders/helloworld/shader.frag.spv");

	//create a graphic pipeline with a specific view port bounding box and  screen bounding box
	GraphicPipeline* pipeline = new GraphicPipeline(vec3f({ 0,0,0 }), vec3f({ float(size.width),float(size.height),1.0f }), vec2f({ 0,0 }), vec2f({ float(size.width),float(size.height) }));
	
	//add the shader modules to the pipeline
	pipeline->setVertexModule(vertexShader);
	pipeline->setFragmentModule(fragmentShader);

	//setup the vertices info for the pipeline
	pipeline->setVerticesInfo(triangle_vertex_buffer->getBindingDescriptions(), triangle_vertex_buffer->getAttributeDescriptions() ,triangle_vertex_buffer->primitiveTopology());
	
	//set the vertex buffer to be rendered
	//the vertex buffer can be change without recompiling the pipeline as long as it is compatible with the informations
	//provided in the previous line
	pipeline->setVertices(triangle_vertex_buffer);

	//describe the subpass in which the pipeline will be rendered
	SubpassAttachment SA;
	//we want to show it on screen
	SA.showOnScreen = true;
	//it has one color output
	SA.nbColor = 1;
	//the color need to be stored after the subpass
	SA.storeColor = true;
	//it uses depth
	SA.useDepth = true;
	//the color output to show on screen will be the first one
	SA.showOnScreenIndex = 0;

	//create the render pass
	RenderPass* pass = new RenderPass();
	//add the subpass in it
	pass->addSubPass({ pipeline }, SA);
	//compile the render pass
	pass->compile();

	//create a framebuffers that match the size of the swapchain images
	FrameBuffer* frameBuffers = new FrameBuffer(s->size().width, s->size().height);
	//setup the framebuffer to be the output of the render pass
	pass->prepareOutputFrameBuffer(*frameBuffers);

	while (w.running()) {
		w.updateInput();
	
		//acquire a swapchain image  
		SwapChainImage& image = s->acquireImage();
		
		//describe how to use the semaphore provided by the image
		std::vector<WaitSemaphoreInfo> wait_semaphore_infos = {};
		wait_semaphore_infos.push_back({
			image.getSemaphore(),                     // VkSemaphore            Semaphore
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT					// VkPipelineStageFlags   WaitingStage
			});

		//we complete the frame buffer with the swapchaine image
		pass->setSwapChainImage(*frameBuffers, image);		

		//wait for the command buffer to be ready
		commandBuffer.wait();
		//reset it's fence
		commandBuffer.resetFence();
		//start adding command into it
		commandBuffer.beginRecord();
		
		//draw call for the render pass
		pass->draw(commandBuffer, *frameBuffers, vec2u({ 0,0 }), vec2u({ size.width, size.height }), { { 0.1f, 0.2f, 0.3f, 1.0f }, { 1.0f, 0 } });
	
		//no more gpu call needed here
		commandBuffer.endRecord();
		//submit the command buffer to a queue
		commandBuffer.submit(queue, wait_semaphore_infos, { commandBuffer.getSemaphore(0) });
		
		//present the image we just drawn into on screen
		s->presentImage(presentQueue, image, { commandBuffer.getSemaphore(0) });
	}
	d->end();
}

```