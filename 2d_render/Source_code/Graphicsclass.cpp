////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "../Header_file/graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Bitmap = 0;
	m_Text = 0;
	

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
		
	if (!Initialize_D3D(screenWidth, screenHeight, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();

	m_Bitmap = new BitmapClass;
	if (!m_Bitmap){
		return false;
	}

	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight,hwnd, L"Resource/sample_img.dds", 256, 256);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK); 
		return false;

	}

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}


	return true;
}



bool GraphicsClass::Initialize_D3D(int screenWidth, int screenHeight, HWND hwnd){

	m_D3D = new D3DClass;

	if (!m_D3D)
	{
		return false;
	}

	if (!m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_Bitmap)
	{ 
		m_Bitmap->Shutdown(); 
		delete
		m_Bitmap; m_Bitmap = 0; 
	}

	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame(int mouseX,int mouseY,int fps,int cpu,float frameTime)
{
	bool result;

	result = m_Text->SetMousePosition(mouseX,mouseY,m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	result = m_Text->setFps(fps, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	result = m_Text->SetCpu(cpu, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}




	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);


	m_D3D->TurnZBufferOff();
	m_D3D->TurnOnAlphaBlending();

	m_Camera->GetViewMatrix(baseViewMatrix);

	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix, baseViewMatrix,256, 256);
	if (!result) 
	{ 
		return false; 
	}


	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix, baseViewMatrix);
	if (!result)
	{
		return false;
	}

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}