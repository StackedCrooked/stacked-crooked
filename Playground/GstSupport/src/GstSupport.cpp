#include "GstSupport/GstSupport.h"
#include "GstSupport/MakeString.h"
#include <gst/interfaces/propertyprobe.h>
#include <stdexcept>


namespace Gst
{

    namespace Pipeline
    {

        GstElement * Create(const char * inName)
        {
            GstElement * pipeline = gst_pipeline_new(inName);
            if (!pipeline)
            {
                throw std::runtime_error("Failed to create a pipeline. ");
            }
            return pipeline;
        }


        GstElement * Parse(const std::string & inPipelineDescription)
        {        
            GstElement * result = 0;
            GError * error(0);
            result = gst_parse_launch(inPipelineDescription.c_str(), &error);
            if (error)
            {
                std::string msg = std::string("Parsing pipeline failed. Reason: ") + error->message;
                throw std::runtime_error(msg);
            }
            return result;
        }


        GstState GetState(GstElement * inElement)
        {
            GstState state(GST_STATE_VOID_PENDING), pending(GST_STATE_VOID_PENDING);
            GstStateChangeReturn ret = gst_element_get_state(inElement, &state, &pending, 1000*1000*1000);
            if (GST_STATE_CHANGE_FAILURE == ret)
            {
                throw std::runtime_error("Failed to obtain the pipeline state. ");
            }
            return state;
        }


        void SetState(GstElement * inElement, GstState inState)
        {
            GstStateChangeReturn ret = gst_element_set_state(inElement, inState);
            if (GST_STATE_CHANGE_FAILURE == ret)
            {
                std::string msg = Gst::MakeString() << "Failed to change the pipeline state to: " << Gst::Logging::ToString(inState) << ". ";
                throw std::runtime_error(msg);
            }
        }

    
        void SendEOS(GstElement * inElement)
        {        
			// This function takes owership of the provided event.
            gst_element_send_event(inElement, gst_event_new_eos());
        }

    } // namespace Pipeline


    namespace Bin
    {
        
        GstElement * GetElementByName(GstBin * inBin, const char * inName)
        {
            ScopedObject<GstElement> result(gst_bin_get_by_name(inBin, inName));
            if (!result)
            {                
                std::string msg = MakeString() << "Bin does not contain an element with name " << inName << ". ";
                throw std::runtime_error(msg);
            }
            return result.release();
        }

    } // namespace Bin


    namespace Element
    {
        
        GstElement * Create(const char * inType, const char * inName)
        {
            GstElement * element = gst_element_factory_make(inType, inName);
            if (!element)
            {
                std::string msg(Gst::MakeString() << "Failed to create element of type " << inType << ". ");
                throw std::runtime_error(msg);
            }   
            return element;
        }

    
        GstPad * GetStaticPad(GstElement * inElement, const char * inName)
        {    
            ScopedObject<GstPad> pad(gst_element_get_static_pad(inElement, inName));
            if (!pad)
            {
                std::string msg(Gst::MakeString() << "Failed to get static pad of type. ");
                throw std::runtime_error(msg);
            }
            return pad.release();
        }
        
        
        GstPad * GetRequestPad(GstElement * inElement, const char * inName)
        {    
            ScopedObject<GstPad> pad(gst_element_get_request_pad(inElement, inName));
            if (!pad)
            {
                std::string msg(Gst::MakeString() << "Failed to get request pad of type. ");
                throw std::runtime_error(msg);
            }
            return pad.release();
        }


        GstPad * GetExistingPadByName(GstElement * inElement, const std::string & inPadName)
        {
            GstPad * result(0);
            
            GstIterator * it = gst_element_iterate_pads(inElement);
            if (!it)
            {
                throw std::runtime_error("Failed to create an iterator for the given element.");
            }

            bool done = false;
            while (!done)
            {
                gpointer pad = 0;
                switch (gst_iterator_next(it, &pad))
                {
                    case GST_ITERATOR_OK:
                    {
                        if (inPadName == GST_PAD_NAME(pad))
                        {
                            result = GST_PAD(pad);
                            done = true;
                        }
                        break;
                    }
                    case GST_ITERATOR_RESYNC:
                    {
                        gst_iterator_resync(it);
                        break;
                    }
                    case GST_ITERATOR_ERROR:
                    {
                        done = true;
                        break;
                    }
                    case GST_ITERATOR_DONE:
                    {
                        done = true;
                        break;
                    }
                };
            }
            gst_iterator_free(it);

            if (!result)
            {
                std::string msg = MakeString() << "Element does not contain a pad with name " << inPadName << ". ";
                throw std::runtime_error(msg);
            }
            return result;
        }


        void Link(GstElement * inLHS, GstElement * inRHS)
        {
            if (!gst_element_link(inLHS, inRHS))
            {   
                std::string msg(Gst::MakeString() << "Failed to link elements " << inLHS->object.name << " and " << inRHS->object.name << ". ");
                throw std::runtime_error(msg);
            }
        }

        void GetDeviceNames(GstElement * inCaptureSrcEl, std::vector<std::string> & outDeviceNames)
        {
            GstPropertyProbe * probe = GST_PROPERTY_PROBE(inCaptureSrcEl);
            GValueArray * values = gst_property_probe_get_values_name(probe, "device-name");
            for (guint i = 0; i < values->n_values; ++i)
            {
                GValue * value = g_value_array_get_nth(values, i);
                if (!value)
                {
                    continue;
                }

                const gchar * stringValue = g_value_get_string(value);
                if (!stringValue)
                {
                    continue;
                }
                
                outDeviceNames.push_back(stringValue);
            }
        }

    } // namespace Element


    namespace Pad
    {    

        void Link(GstPad * inLHS, GstPad * inRHS)
        {
            GstPadLinkReturn ret = gst_pad_link(inLHS, inRHS);
            if (ret != GST_PAD_LINK_OK)
            {
                std::string msg(Gst::MakeString() << "Failed to link pads. Reason: " << Logging::ToString(ret) << ". ");
                throw std::runtime_error(msg);
            }
        }


        bool IsAudioPad(GstPad * inPad)
        {
            ScopedObject<GstCaps> caps(gst_pad_get_caps(inPad));
            std::string capsName = Caps::GetName(caps.get());
            return NULL != g_strrstr(capsName.c_str(), "audio");
        }


        bool IsVideoPad(GstPad * inPad)
        {
            ScopedObject<GstCaps> caps(gst_pad_get_caps(inPad));
            std::string capsName = Caps::GetName(caps.get());
            return NULL != g_strrstr(capsName.c_str(), "video");
        }

    } // namespace Pad


    namespace Caps
    {

        std::string GetName(GstCaps * inCaps)
        {
            return gst_structure_get_name(gst_caps_get_structure(inCaps, 0));
        }

    } // namespace Caps


    namespace Logging
    {

        bool IsLogMessage(GstMessage * msg)
        {
            switch (msg->type)
            {
                case GST_MESSAGE_INFO:
                case GST_MESSAGE_WARNING:
                case GST_MESSAGE_ERROR:
                {
                    return true;
                }
                default:
                {
                    return false;
                }
            }
        }

    
        std::string GetLogMessage(GstMessage * msg)
        {
            std::string ret;
            gchar * debug(0);
            GError * error(0);

            switch (msg->type)
            {
                case GST_MESSAGE_INFO:
                {
                    gst_message_parse_info(msg, &error, &debug);
                    break;
                }
                case GST_MESSAGE_WARNING:
                {
                    gst_message_parse_warning(msg, &error, &debug);
                    break;
                }
                case GST_MESSAGE_ERROR:
                {
                    gst_message_parse_error(msg, &error, &debug);
                    break;
                }
                default:
                {
                    ret = "This message is not of type info, warning or error. ";
                    break;
                }
            }

            if (error)
            {
                ret += std::string(error->message) + ". ";
            }

            if (debug)
            {
                ret += "Debug string: " + std::string(debug) + ". ";
            }

            g_free(debug);
            g_error_free(error);
            return ret;
        }


        const char * ToString(GstState inState)
        {
            switch (inState)
            {
                case GST_STATE_VOID_PENDING:
                {
                    return "GST_STATE_VOID_PENDING";
                }
                case GST_STATE_NULL:
                {
                    return "GST_STATE_NULL";
                }
                case GST_STATE_READY:
                {
                    return "GST_STATE_READY";
                }
                case GST_STATE_PAUSED:
                {
                    return "GST_STATE_PAUSED";
                }
                case GST_STATE_PLAYING:
                {
                    return "GST_STATE_PLAYING";
                }
                default:
                {
                    return "Unknown state!";
                }
            }
        }

        
        GstState FromString(const std::string & inState)
        {
            if (inState == "GST_STATE_VOID_PENDING")
            {
                return GST_STATE_VOID_PENDING;
            }
            else if (inState == "GST_STATE_NULL")
            {
                return GST_STATE_NULL;
            }
            else if (inState == "GST_STATE_READY")
            {
                return GST_STATE_READY;
            }
            else if (inState == "GST_STATE_PAUSED")
            {
                return GST_STATE_PAUSED;
            }
            else if (inState == "GST_STATE_PLAYING")
            {
                return GST_STATE_PLAYING;
            }

            std::string msg = "Invalid state: " + inState;
            throw std::runtime_error(msg);
        }


        const char * ToString(GstMessageType inMessageType)
        {
            switch (inMessageType)
            {
                case GST_MESSAGE_UNKNOWN: return "GST_MESSAGE_UNKNOWN";         
                case GST_MESSAGE_EOS: return "GST_MESSAGE_EOS";
                case GST_MESSAGE_ERROR: return "GST_MESSAGE_ERROR";
                case GST_MESSAGE_WARNING: return "GST_MESSAGE_WARNING";
                case GST_MESSAGE_INFO: return "GST_MESSAGE_INFO";
                case GST_MESSAGE_TAG: return "GST_MESSAGE_TAG";
                case GST_MESSAGE_BUFFERING: return "GST_MESSAGE_BUFFERING";
                case GST_MESSAGE_STATE_CHANGED: return "GST_MESSAGE_STATE_CHANGED";
                case GST_MESSAGE_STATE_DIRTY: return "GST_MESSAGE_STATE_DIRTY";
                case GST_MESSAGE_STEP_DONE: return "GST_MESSAGE_STEP_DONE";
                case GST_MESSAGE_CLOCK_PROVIDE: return "GST_MESSAGE_CLOCK_PROVIDE";
                case GST_MESSAGE_CLOCK_LOST: return "GST_MESSAGE_CLOCK_LOST";
                case GST_MESSAGE_NEW_CLOCK: return "GST_MESSAGE_NEW_CLOCK";
                case GST_MESSAGE_STRUCTURE_CHANGE: return "GST_MESSAGE_STRUCTURE_CHANGE";
                case GST_MESSAGE_STREAM_STATUS: return "GST_MESSAGE_STREAM_STATUS";
                case GST_MESSAGE_APPLICATION: return "GST_MESSAGE_APPLICATION";
                case GST_MESSAGE_ELEMENT: return "GST_MESSAGE_ELEMENT";
                case GST_MESSAGE_SEGMENT_START: return "GST_MESSAGE_SEGMENT_START";
                case GST_MESSAGE_SEGMENT_DONE: return "GST_MESSAGE_SEGMENT_DONE";
                case GST_MESSAGE_DURATION: return "GST_MESSAGE_DURATION";
                case GST_MESSAGE_LATENCY: return "GST_MESSAGE_LATENCY";
                case GST_MESSAGE_ASYNC_START: return "GST_MESSAGE_ASYNC_START";
                case GST_MESSAGE_ASYNC_DONE: return "GST_MESSAGE_ASYNC_DONE";
                case GST_MESSAGE_REQUEST_STATE: return "GST_MESSAGE_REQUEST_STATE";
                case GST_MESSAGE_STEP_START: return "GST_MESSAGE_STEP_START";
                case GST_MESSAGE_ANY: return "GST_MESSAGE_ANY";
                default: return "Invalid value!";
            }
        }


        const char * ToString(GstPadLinkReturn inPadLinkReturn)
        {
            switch (inPadLinkReturn)
            {
                case GST_PAD_LINK_OK:
                {
                    return "GST_PAD_LINK_OK";
                }
                case GST_PAD_LINK_WRONG_HIERARCHY:
                {
                    return "GST_PAD_LINK_WRONG_HIERARCHY";
                }
                case GST_PAD_LINK_WAS_LINKED:
                {
                    return "GST_PAD_LINK_WAS_LINKED";
                }
                case GST_PAD_LINK_WRONG_DIRECTION:
                {
                    return "GST_PAD_LINK_WRONG_DIRECTION";
                }
                case GST_PAD_LINK_NOFORMAT:
                {
                    return "GST_PAD_LINK_NOFORMAT";
                }
                case GST_PAD_LINK_NOSCHED:
                {
                    return "GST_PAD_LINK_NOSCHED";
                }
                case GST_PAD_LINK_REFUSED:
                {
                    return "GST_PAD_LINK_REFUSED";
                }
                default:
                {
                    return "Invalid GstPadLinkReturn value!";
                }
            }
        }


        const char * ToString(GstStreamStatusType inGstStreamStatusType)
        {
            switch (inGstStreamStatusType)
            {
                case GST_STREAM_STATUS_TYPE_CREATE:
                {
                    return "GST_STREAM_STATUS_TYPE_CREATE";
                }
                case GST_STREAM_STATUS_TYPE_ENTER:
                {
                    return "GST_STREAM_STATUS_TYPE_ENTER";
                }
                case GST_STREAM_STATUS_TYPE_LEAVE:
                {
                    return "GST_STREAM_STATUS_TYPE_LEAVE";
                }
                case GST_STREAM_STATUS_TYPE_DESTROY:
                {
                    return "GST_STREAM_STATUS_TYPE_DESTROY";
                }
                case GST_STREAM_STATUS_TYPE_START:
                {
                    return "GST_STREAM_STATUS_TYPE_START";
                }
                case GST_STREAM_STATUS_TYPE_PAUSE:
                {
                    return "GST_STREAM_STATUS_TYPE_PAUSE";
                }
                case GST_STREAM_STATUS_TYPE_STOP:
                {
                    return "GST_STREAM_STATUS_TYPE_STOP";
                }
                default:
                {
                    return "Invalid value!";
                }
            }
        }


        const char * ToString(GstStateChangeReturn inGstStateChangeReturn)
        {
            switch (inGstStateChangeReturn)
            {
                case GST_STATE_CHANGE_FAILURE:
                {
                    return "GST_STATE_CHANGE_FAILURE";
                }
                case GST_STATE_CHANGE_SUCCESS:
                {
                    return "GST_STATE_CHANGE_SUCCESS";
                }
                case GST_STATE_CHANGE_ASYNC:
                {
                    return "GST_STATE_CHANGE_ASYNC";
                }
                case GST_STATE_CHANGE_NO_PREROLL:
                {
                    return "GST_STATE_CHANGE_NO_PREROLL";
                }
                default:
                {
                    return "Invalid value!";
                }
            }
        }

    } // namespace Logging

} // namespace Gst
