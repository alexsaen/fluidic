/*
Copyright (c) 2010, Steven Leigh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name "Fluidic" nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Steven Leigh BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once;

#include "Fluidic.h"

namespace TestFluidic
{
	class TestObject : public Fluidic::IVelocityPoller
	{
	public:
		TestObject(Fluidic::Vector p, Fluidic::Vector v, float speedUp) 
			: IVelocityPoller(p), mVelocity(v), mFluidVelocity(v), mSpeedUp(speedUp), mInitialPosition(p)
		{}

		void Update(float dt) 
		{
			//move object
			if (dt > 0)
			{
				mVelocity += (mFluidVelocity - mVelocity) * mSpeedUp * dt;
				//err towards the start position
				mVelocity += (mInitialPosition - mPosition) * mSpeedUp * dt * 0.5f;
				mPosition += mVelocity * dt;				
			}
		}

		void UpdateVelocity(const Fluidic::Vector &v)
		{
			mFluidVelocity = v;
		}

		float mSpeedUp;
		Fluidic::Vector mVelocity;
		Fluidic::Vector mFluidVelocity;
	protected:
		Fluidic::Vector mInitialPosition;
	};
}