/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Crimild.hpp>
#include <Crimild_GLFW.hpp>

using namespace crimild;
using namespace crimild::messaging;

static SharedPointer< Simulation > sim;

class ViewControls : 
    public NodeComponent,
    public Messenger {
public:
    ViewControls( void )
    {
        auto self = this;

        registerMessageHandler< MouseButtonDown >( [self]( MouseButtonDown const &msg ) {
            self->_lastMousePos = Input::getInstance()->getNormalizedMousePosition();
        });

        registerMessageHandler< MouseMotion >( [self]( MouseMotion const &msg ) {
            if ( Input::getInstance()->isMouseButtonDown( CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) ) {
                auto currentMousePos = Input::getInstance()->getNormalizedMousePosition();
                auto delta = currentMousePos - self->_lastMousePos;
                self->_lastMousePos = Input::getInstance()->getNormalizedMousePosition();

                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ) {
                    self->translateView( Vector3f( 3.0f * delta[ 0 ], -3.0f * delta[ 1 ], 0.0f ) );
                }
                else {
                    self->rotateView( Vector3f( delta[ 1 ], 3.0f * delta[ 0 ], 0.0f ) );
                }
            }
        });

        registerMessageHandler< MouseButtonUp >( [self]( MouseButtonUp const &msg ) {

        });

        registerMessageHandler< MouseScroll >( [self]( MouseScroll const &msg ) {
            self->getNode()->local().translate() += 0.1f * msg.dy * Vector3f( 0.0f, 0.0f, 1.0f );
        });
    }

    virtual ~ViewControls( void )
    {

    }

    virtual void update( const Clock &clock ) override
    {
        bool shouldTranslate = Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT );
        bool translateSpeed = 0.5f * clock.getDeltaTime();

        if ( Input::getInstance()->isKeyDown( 'W' ) ) {
            if ( shouldTranslate ) {
                translateView( Vector3f( 0.0f, -translateSpeed, 0.0f ) );
            }
            else {
                rotateView( Vector3f( -0.1f, 0.0f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'S' ) ) {
            if ( shouldTranslate ) {
                translateView( Vector3f( 0.0f, translateSpeed, 0.0f ) );
            }
            else {
                rotateView( Vector3f( 0.1f, 0.0f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'A' ) ) {
            if ( shouldTranslate ) {
                translateView( Vector3f( translateSpeed, 0.0f, 0.0f ) );
            }
            else {
                rotateView( Vector3f( 0.0f, -0.1f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'D' ) ) {
            if ( shouldTranslate ) {
                translateView( Vector3f( -translateSpeed, 0.0f, 0.0f ) );
            }
            else {
                rotateView( Vector3f( 0.0f, 0.1f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'Q' ) ) {
            rotateView( Vector3f( 0.0f, 0.0f, 0.1f ) );
        }

        if ( Input::getInstance()->isKeyDown( 'E' ) ) {
            rotateView( Vector3f( 0.0f, 0.0f, -0.1f ) );
        }
    }

private:
    void translateView( const Vector3f &delta )
    {
        getNode()->local().translate() += delta;
    }

    void rotateView( const Vector3f &delta )
    {
        if ( delta[ 0 ] != 0.0f ) {
            Vector3f xAxis( 1.0f, 0.0f, 0.0f );
            getNode()->getLocal().applyInverseToVector( Vector3f( 1.0f, 0.0f, 0.0f ), xAxis );
            getNode()->local().rotate() *= Quaternion4f::createFromAxisAngle( xAxis, delta[ 0 ] );
        }

        if ( delta[ 1 ] != 0.0f ) {
            Vector3f yAxis( 0.0f, 1.0f, 0.0f );
            getNode()->getLocal().applyInverseToVector( Vector3f( 0.0f, 1.0f, 0.0f ), yAxis );
            getNode()->local().rotate() *= Quaternion4f::createFromAxisAngle( yAxis, delta[ 1 ] );
        }

        if ( delta[ 2 ] != 0.0f ) {
            Vector3f zAxis( 0.0f, 0.0f, 1.0f );
            getNode()->getLocal().applyInverseToVector( Vector3f( 0.0f, 0.0f, 1.0f ), zAxis );
            getNode()->local().rotate() *= Quaternion4f::createFromAxisAngle( zAxis, delta[ 2 ] );
        }
    }

private:
    Vector2f _lastMousePos;
};

int main( int argc, char **argv )
{
	sim = crimild::alloc< GLSimulation >( "Triangle", crimild::alloc< Settings >( argc, argv ) );

	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ) );

    auto scene = crimild::alloc< Group >();

    auto camera = crimild::alloc< Camera >();
    camera->local().setTranslate( Vector3f( 0.0f, 0.0f, 3.0f ) );
    scene->attachNode( camera );

	std::string modelPath = FileSystem::getInstance().pathForResource( "assets/astroboy.crimild" );
	FileStream is( modelPath, FileStream::OpenMode::READ );
	is.load();
	if ( is.getObjectCount() > 0 ) {
		auto model = is.getObjectAt< Node >( 0 );
		if ( model != nullptr ) {
			model->perform( UpdateWorldState() );

			const auto SCALE = 1.0f / model->getWorldBound()->getRadius();
			model->local().setScale( SCALE );
			model->local().translate() -= SCALE * Vector3f( 0.0f, model->getWorldBound()->getCenter()[ 1 ], 0.0f );
			
			auto pivot = crimild::alloc< Group >();
			pivot->attachNode( model );
			pivot->attachComponent< ViewControls >();
			scene->attachNode( pivot );
			
			scene->attachNode( pivot );
		}
	}

	auto light = crimild::alloc< Light >( Light::Type::POINT );
	light->local().setTranslate( 1.0f, 1.0f, 1.0f );
	light->setAmbient( RGBAColorf::ZERO );
	scene->attachNode( light );

    sim->setScene( scene );

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
	sim->start();
#else
	sim->run();
	sim = nullptr;
	return 0;
#endif
}

