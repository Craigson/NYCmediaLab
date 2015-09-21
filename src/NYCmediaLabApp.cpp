#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Cinder-OpenNI.h"
#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NYCmediaLabApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    ci::Channel16u				mChannel;
    OpenNI::DeviceRef			mDevice;
    OpenNI::DeviceManagerRef	mDeviceManager;
    ci::gl::TextureRef			mTexture;
    void						onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions );
};

void NYCmediaLabApp::setup()
{
    // The device manager automatically initializes OpenNI and NiTE.
    // It's a good idea to check that initialization is complete
    // before accessing devices.
    mDeviceManager		= OpenNI::DeviceManager::create();
    
    if ( mDeviceManager->isInitialized() ) {
        
        // Catching this exception prevents our application
        // from crashing when no devices are connected.
        try {
            mDevice = mDeviceManager->createDevice(OpenNI::DeviceOptions().enableDepth(true));
        } catch ( OpenNI::ExcDeviceNotAvailable ex ) {
            console() << ex.what() << endl;
            quit();
            return;
        }
        
        // If we've successfully accessed a device, start and add a
        // callback for the depth stream.
        if ( mDevice ) {
            mDevice->connectDepthEventHandler( &NYCmediaLabApp::onDepth, this );
            mDevice->start();
        }
    }
}

void NYCmediaLabApp::mouseDown( MouseEvent event )
{
}

void NYCmediaLabApp::update()
{
 //   std::cout << mDevice.getResolutionX() << std::endl;
    
}
void NYCmediaLabApp::onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
{
    mChannel = OpenNI::toChannel16u( frame );
}

void NYCmediaLabApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::color(1.,1.,1.);
    	gl::setMatricesWindow( getWindowSize() );
    
        if ( mTexture ) {
            mTexture->update( Channel32f( mChannel ) );
        } else {
            mTexture = gl::Texture::create( Channel32f( mChannel ) );
        }
        gl::draw( mTexture, mTexture->getBounds(), getWindowBounds() );
}

CINDER_APP( NYCmediaLabApp, RendererGl )
