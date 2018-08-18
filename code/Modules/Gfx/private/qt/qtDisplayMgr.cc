//------------------------------------------------------------------------------
//  displayMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "qtDisplayMgr.h"
#include "Core/Core.h"
#include "Gfx/private/gl/gl_impl.h"
#include "Gfx/private/gl/glCaps.h"

namespace Oryol {
	namespace _priv {
		qtDisplayMgr* qtDisplayMgr::self = nullptr;

		qtDisplayMgr::qtDisplayMgr() {
			o_assert(nullptr == self);
			self = this;
		}

		//------------------------------------------------------------------------------
		qtDisplayMgr::~qtDisplayMgr() {
			o_assert(!this->displayValid);
		}

		//------------------------------------------------------------------------------
		/**
		The SetupDisplay() method is expected to initialize everything necessary
		for rendering (e.g. creating the application window, create a GL context,...).
		It takes a DisplaySetup object which contains setup parameters like
		width, height, pixelformat, window title, etc...
		This method must be overwritten in a platform-specific subclass.
		*/
		void
			qtDisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
			o_assert(!this->displayValid);

			o_assert(!this->IsDisplayValid());

			displayMgrBase::SetupDisplay(setup, ptrs);

			// setup extensions and platform-dependent constants
			ORYOL_GL_CHECK_ERROR();
		#if ORYOL_QT_EDITOR
			flextInit();
		#endif
			//void flextLoadOpenGLFunctions();
			ORYOL_GL_CHECK_ERROR();
			glCaps::Setup(glCaps::GL_3_3_CORE);
		#if ORYOL_DEBUG
			glCaps::EnableDebugOutput(glCaps::SeverityMedium);
		#endif

			this->displayValid = true;
			this->gfxSetup = setup;
			this->displayAttrs = setup.GetDisplayAttrs();
			this->pointers = ptrs;
			this->curFramebufferWidth = this->displayAttrs.FramebufferWidth;
			this->curFramebufferHeight = this->displayAttrs.FramebufferHeight;
		}

		//------------------------------------------------------------------------------
		void
			qtDisplayMgr::DiscardDisplay() {
			o_assert(this->displayValid);
			this->displayValid = false;
			this->pointers = gfxPointers();

			glCaps::Discard();
			displayMgrBase::DiscardDisplay();
		}

		//------------------------------------------------------------------------------
		bool
			qtDisplayMgr::IsDisplayValid() const {
			return this->displayValid;
		}

		//------------------------------------------------------------------------------
		void
			qtDisplayMgr::ModifyDisplay(const GfxSetup& setup) {
			o_assert(this->displayValid);
			this->displayAttrs = setup.GetDisplayAttrs();
			this->notifyEventHandlers(GfxEvent(GfxEvent::DisplayModified, this->displayAttrs));
		}

		//------------------------------------------------------------------------------
		/**
		This method is expected to process the platform specific window system
		messages. This is also usually the place where input events from the
		host system are consumed and forwarded to the Oryol input system. The method
		is usually called at the start of a new frame, and must be overwritten
		by platform-specific subclasses.
		*/
		void
			qtDisplayMgr::ProcessSystemEvents() {
			if ((this->curFramebufferWidth != this->displayAttrs.FramebufferWidth) ||
				(this->curFramebufferHeight != this->displayAttrs.FramebufferHeight)) {

				this->curFramebufferWidth = this->displayAttrs.FramebufferWidth;
				this->curFramebufferHeight = this->displayAttrs.FramebufferHeight;
				this->notifyEventHandlers(GfxEvent(GfxEvent::DisplayModified, this->displayAttrs));
			}
		}

		//------------------------------------------------------------------------------
		/**
		This method will be called when all rendering commands for the current
		frames has been issued and the result should be presented. Must be
		overwritten in platform-specific subclass.
		*/
		void
			qtDisplayMgr::Present() {
			// empty
		}

		//------------------------------------------------------------------------------
		bool
			qtDisplayMgr::QuitRequested() const {
			return false;
		}

		//------------------------------------------------------------------------------
		void qtDisplayMgr::glBindDefaultFramebuffer() {}

		//------------------------------------------------------------------------------
		const DisplayAttrs&
			qtDisplayMgr::GetDisplayAttrs() const {
			return this->displayAttrs;
		}

		//------------------------------------------------------------------------------
		qtDisplayMgr::eventHandlerId
			qtDisplayMgr::Subscribe(eventHandler handler) {
			eventHandlerId id = this->uniqueIdCounter++;
			this->handlers.Add(id, handler);
			return id;
		}

		//------------------------------------------------------------------------------
		void
			qtDisplayMgr::Unsubscribe(eventHandlerId id) {
			if (this->handlers.Contains(id)) {
				this->handlers.Erase(id);
			}
		}

		//------------------------------------------------------------------------------
		void
			qtDisplayMgr::notifyEventHandlers(const GfxEvent& gfxEvent) {
			for (const auto& handler : this->handlers) {
				handler.Value()(gfxEvent);
			}
		}

		void qtDisplayMgr::setFrameBufferSize(int Width, int Height) {
			// update display attributes (ignore window-minimized)
			if ((Width != 0) && (Height != 0)) {
				self->displayAttrs.FramebufferWidth = Width;
				self->displayAttrs.FramebufferHeight = Height;
			}
		}

	} // namespace _priv
} // namespace Oryol
