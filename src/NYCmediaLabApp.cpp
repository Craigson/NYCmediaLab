#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Cinder-OpenNI.h"
#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"
#include "CinderOpenCV.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NYCmediaLabApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    cv::Mat mPreviousFrame;
    cv::Mat mBackground;
    Color mColor;

    
private:
    ci::Channel16u				mDepthChannel;
    ci::Channel8u               mColourChannel;
    ci::Channel8u               mIRchannel;
    OpenNI::DeviceRef			mDevice;
    OpenNI::DeviceManagerRef	mDeviceManager;
    ci::gl::TextureRef			mTexture;
    ci::Surface8u               mSurface;
    
    gl::TextureRef mTextureDepth;
    ci::Surface8u mSurfaceDepth;
    
    void						onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions );
    void						onColor( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions );
    void                        onIR ( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions );

    
    
    
    std::list<float> depthData;
};

void NYCmediaLabApp::setup()
{
    // The device manager automatically initializes OpenNI and NiTE.
    // It's a good idea to check that initialization is complete
    // before accessing devices.
    
    mColor = Color(1, 0, 0);
    
    mDeviceManager		= OpenNI::DeviceManager::create();
    
    if ( mDeviceManager->isInitialized() ) {
        
        // Catching this exception prevents our application
        // from crashing when no devices are connected.
        try {
            mDevice = mDeviceManager->createDevice();
        } catch ( OpenNI::ExcDeviceNotAvailable ex ) {
            console() << ex.what() << endl;
            quit();
            return;
        }
        
        // If we've successfully accessed a device, start and add a
        // callback for the depth stream.
        if ( mDevice ) {
            mDevice->connectDepthEventHandler( &NYCmediaLabApp::onDepth, this );
            //mDevice->connectColorEventHandler(&NYCmediaLabApp::onDepth, this);
            mPreviousFrame = cv::Mat( 240,320, CV_16UC1 );
            mBackground = cv::Mat( 240,320, CV_16UC1 );
            mDevice->start();
        }
    }
}

void NYCmediaLabApp::mouseDown( MouseEvent event )
{
}

void NYCmediaLabApp::update()
{
  //  std::cout << "width: " << mDepthChannel.getWidth() << "  height: " << mDepthChannel.getHeight() << std::endl;
    
    /*
     mChannel.getData(int x, int y) RETURNS A POINTER TO THE ADDRESS IN MEMORY WHERE THE VALUE OF THE POSITION
     IS STORED.  IE, DEREFERENCE THE POINTER TO OBTAIN THE VALUE
     */
   // std::cout << *(mDepthChannel.getData(160,120))  << std::endl;
    
}
void NYCmediaLabApp::onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
{
   // mDepthChannel = OpenNI::toChannel16u( frame );
    
    cv::Mat mInput = toOcv( OpenNI::toChannel16u( frame ) );
    cv::Mat mInput8bit;
    cv::Mat mSubtracted;
    cv::Mat mThreshold;
    cv::Mat mOutput;
    
    
    // subtrackted depth
    cv::absdiff( mBackground, mInput, mSubtracted );
    
    mSubtracted.convertTo( mThreshold, CV_8UC1 );
    
    cv::adaptiveThreshold(mThreshold, mThreshold, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 105, 1);
    cv::threshold( mThreshold, mThreshold, 128, 255, cv::THRESH_BINARY );
    //cv::dilate( mThreshold, mThreshold, cv::Mat(), cv::Point( -1, -1), 2, 1, 1 );
    cv::cvtColor( mThreshold,mThreshold, CV_GRAY2RGB );
    
    mThreshold.copyTo( mOutput );
    
    mInput.copyTo( mPreviousFrame );
    
    // display the threshold image
    mSurfaceDepth = Surface8u( fromOcv( mOutput ) );
    
}

void NYCmediaLabApp::onColor( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
{
    mColourChannel = OpenNI::toChannel8u( frame );
}

void NYCmediaLabApp::onIR( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
{
    mIRchannel = OpenNI::toChannel8u( frame );
}

void NYCmediaLabApp::draw()
{
    /*
	gl::clear( Color( 0, 0, 0 ) );
//    gl::color(1.,1.,1.);
    	gl::setMatricesWindow( getWindowSize() );
    
        if ( mTexture ) {
            mTexture->update( Channel32f( mColourChannel ) );
        } else {
            mTexture = gl::Texture::create( Channel32f( mColourChannel ) );
        }
        gl::draw( mTexture, mTexture->getBounds(), getWindowBounds() );
    */
    
    gl::setMatricesWindow( getWindowSize() );
    gl::clear( Colorf::black() );
    
//        if ( mTexture ) {
//            mTexture->update(Channel8u(mDepthChannel));
//        } else {
//            mTexture = gl::Texture::create(Channel8u( mDepthChannel ) );
//        }
//        gl::draw( mTexture, mTexture->getBounds(), getWindowBounds() );


            if( mTextureDepth ) {
                mTextureDepth->update( mSurfaceDepth );
            } else {
                mTextureDepth = gl::Texture::create( mSurfaceDepth );
            }
            gl::draw( mTextureDepth, mTextureDepth->getBounds(), getWindowBounds() );

}

CINDER_APP( NYCmediaLabApp, RendererGl )
