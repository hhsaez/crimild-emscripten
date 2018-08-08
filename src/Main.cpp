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
#include <Crimild_SDL.hpp>

using namespace crimild;
using namespace crimild::audio;
using namespace crimild::sdl;
using namespace crimild::animation;
using namespace crimild::messaging;

class ViewControls : 
    public NodeComponent,
    public Messenger {
	CRIMILD_IMPLEMENT_RTTI( ViewControls );
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

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
#define SIM_LIFETIME static
#else
#define SIM_LIFETIME
#endif

int main( int argc, char **argv )
{
	crimild::init();

	SIM_LIFETIME auto sim = crimild::alloc< SDLSimulation >( "Crimild Emscripten Demo", crimild::alloc< Settings >( argc, argv ) );

	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ) );

    auto scene = crimild::alloc< Group >();

    auto camera = crimild::alloc< Camera >();
    camera->local().setTranslate( Vector3f( 0.0f, 0.0f, 1.25f ) );
    scene->attachNode( camera );

	coding::FileDecoder decoder;
	std::string modelPath = FileSystem::getInstance().pathForResource( "assets/astroboy.crimild" );
	if ( decoder.read( modelPath ) ) {
		if ( decoder.getObjectCount() > 0 ) {
			auto model = decoder.getObjectAt< Node >( 0 );
			if ( model != nullptr ) {
				model->perform( UpdateWorldState() );

				const auto SCALE = 1.0f / model->getWorldBound()->getRadius();
				model->local().setScale( SCALE );
				model->local().translate() -= SCALE * Vector3f( 0.0f, model->getWorldBound()->getCenter()[ 1 ], 0.0f );
			
				auto pivot = crimild::alloc< Group >();
				pivot->attachNode( model );
				pivot->attachComponent< ViewControls >();

				model->perform( ApplyToGeometries( []( Geometry *g ) {
					if ( auto ms = g->getComponent< MaterialComponent >() ) {
						ms->forEachMaterial( []( Material *m ) {
							m->setAmbient( RGBAColorf::ZERO );
						});
					}
				}));

				scene->attachNode( pivot );

				if ( auto skeleton = model->getComponent< Skeleton >() ) {
					if ( skeleton->getClips().size() > 0 ) {
						auto animation = crimild::alloc< Animation >( skeleton->getClips().values().first() );
						
						auto centerAnim = crimild::alloc< Animation >(
							crimild::alloc< Clip >(
								"center",
								crimild::alloc< Quaternion4fChannel >(
									"astroBoy_newSkeleton_neck01[r]",
									containers::Array< crimild::Real32 > { 2.0 },
									containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Y, 0.0 ) }
									)
								)
							);
						
						auto leftAnim = crimild::alloc< Animation >(
							crimild::alloc< Clip >(
								"left",
								crimild::alloc< Quaternion4fChannel >(
									"astroBoy_newSkeleton_neck01[r]",
									containers::Array< crimild::Real32 > { 2.0 },
									containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Z, -0.45 * Numericf::PI ) }
									)
								)
							);
						
						auto rightAnim = crimild::alloc< Animation >(
							crimild::alloc< Clip >(
								"right",
								crimild::alloc< Quaternion4fChannel >(
									"astroBoy_newSkeleton_neck01[r]",
									containers::Array< crimild::Real32 > { 2.0 },
									containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Z, 0.45 * Numericf::PI ) }
									)
								)
							);
						
						auto upAnim = crimild::alloc< Animation >(
							crimild::alloc< Clip >(
								"up",
								crimild::alloc< Quaternion4fChannel >(
									"astroBoy_newSkeleton_neck01[r]",
									containers::Array< crimild::Real32 > { 2.0 },
									containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Y, -0.45 * Numericf::PI ) }
									)
								)
							);
						
						auto downAnim = crimild::alloc< Animation >(
							crimild::alloc< Clip >(
								"down",
								crimild::alloc< Quaternion4fChannel >(
									"astroBoy_newSkeleton_neck01[r]",
									containers::Array< crimild::Real32 > { 2.0 },
									containers::Array< Quaternion4f > { Quaternion4f::createFromAxisAngle( Vector3f::UNIT_Y, 0.45 * Numericf::PI ) }
									)
								)
							);

						model->attachComponent< LambdaComponent >( [ animation, skeleton, centerAnim, leftAnim, rightAnim, upAnim, downAnim ]( Node *node, const Clock &c ) {
							auto mousePos = Input::getInstance()->getNormalizedMousePosition();
							
							auto xLeft = Numericf::clamp( 2.0f * mousePos.x(), 0.0f, 1.0f );
							auto xRight = Numericf::clamp( 2.0f * ( mousePos.x() - 0.5f ), 0.0f, 1.0f );
							leftAnim->update( c )->lerp( centerAnim, xLeft )->lerp( rightAnim, xRight );
							
							auto yUp = Numericf::clamp( mousePos.y() / 0.3f, 0.0f, 1.0f );
							auto yDown = Numericf::clamp( ( mousePos.y() - 0.3f ) / 0.7f, 0.0f, 1.0f );
							upAnim->update( c )->lerp( centerAnim, yUp )->lerp( downAnim, yDown );
							
							leftAnim->lerp( upAnim, 0.5f, false );

							animation->update( c )->add( leftAnim, 1.0f );
							skeleton->animate( crimild::get_ptr( animation ) );
							node->perform( UpdateWorldState() );
						});
					}
				}
			}
		}
	}

	auto light = crimild::alloc< Light >( Light::Type::POINT );
	light->local().setTranslate( 1.0f, 1.0f, 1.0f );
	scene->attachNode( light );
	
	auto light2 = crimild::alloc< Light >( Light::Type::POINT );
	light2->local().setTranslate( -1.0f, -1.0f, -1.0f );
	scene->attachNode( light2 );
	
	auto light3 = crimild::alloc< Light >( Light::Type::POINT );
	light3->local().setTranslate( 0.0f, 0.0f, 3.0f );
	light3->setAttenuation( Vector3f( 2.0f, 0.0f, 0.0f ) );
	scene->attachNode( light3 );

    scene->attachComponent< AudioSourceComponent >( AudioManager::getInstance()->createAudioSource( FileSystem::getInstance().pathForResource( "assets/music/steps.wav" ), false ) );
	auto source = scene->getComponent< AudioSourceComponent >()->getAudioSource();
	if ( source != nullptr ) {
		source->setLoop( true );
		source->setVolume( 0.5f );
		source->play();
	}
	
    sim->setScene( scene );

	return sim->run();
}

