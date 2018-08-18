#pragma once
//------------------------------------------------------------------------------
/**
@class Oryol::_priv::qtDisplayMgr
@ingroup _priv
@brief qt offscreen 

@see displayMgr
*/
#include "Gfx/GfxTypes.h"
#include "Core/Containers/Map.h"
#include "Gfx/private/gfxPointers.h"
#include "Gfx/private/displayMgrBase.h"
#include <functional>

namespace Oryol {
	namespace _priv {

		class qtDisplayMgr : public displayMgrBase {
		public:
			/// event handler typedef
			typedef std::function<void(const GfxEvent&)> eventHandler;
			/// event handler id
			typedef unsigned int eventHandlerId;

			qtDisplayMgr();

			/// destructor
			~qtDisplayMgr();

			/// setup the display system, must happen before rendering
			void SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs);
			/// discard the display, rendering cannot happen after
			void DiscardDisplay();
			/// return true if display is currently setup
			bool IsDisplayValid() const;
			/// modify the display settings, may not be supported on all platforms
			void ModifyDisplay(const GfxSetup& setup);
			/// process window system events (call near start of frame)
			void ProcessSystemEvents();
			/// present the current rendered frame
			void Present();
			/// check whether the window system requests to quit the application
			bool QuitRequested() const;

			/// bind the default frame buffer
			void glBindDefaultFramebuffer();

			/// get actual display attributes (can be different from DisplaySetup)
			const DisplayAttrs& GetDisplayAttrs() const;

			/// subscribe to display events
			eventHandlerId Subscribe(eventHandler handler);
			/// unsubscribe from display events
			void Unsubscribe(eventHandlerId id);

			static void setFrameBufferSize(int Width, int Height);

			static qtDisplayMgr *self;

		protected:
			/// notify event handlers, all handlers get the same message object
			void notifyEventHandlers(const GfxEvent& gfxEvent);

			GfxSetup gfxSetup;
			DisplayAttrs displayAttrs;
			eventHandlerId uniqueIdCounter = 0;
			Map<eventHandlerId, eventHandler> handlers;
			gfxPointers pointers;
			bool displayValid = false;
			int curFramebufferWidth = 0;    // used to detect display size changes
			int curFramebufferHeight = 0;   // used to detect display size changes
		};

	} // namespace _priv
} // namespace Oryol