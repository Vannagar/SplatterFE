#include "CopilotClient.h"

CopilotClient::CopilotClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(copilot::AnimationCopilot::NewStub(channel)) {}

#include <iostream>

std::string CopilotClient::ExecutePrompt(const std::string& prompt) {
    copilot::PromptRequest request;
    request.set_prompt(prompt);

    // Mock constructing the detailed scene graph using protobuf structures
    copilot::Context* context = request.mutable_context();
    context->set_document_id("mock_doc_1");
    context->set_selected_layer_id("layer_1");
    
    // Add a mock layer so the backend Copilot can reason about the current state
    copilot::DocumentLayer* layer1 = context->add_layers();
    layer1->set_id("layer_1");
    layer1->set_name("Red Circle");
    layer1->set_layer_type("ellipse");
    (*layer1->mutable_properties())["position.x"] = 0.0f;
    (*layer1->mutable_properties())["position.y"] = 0.0f;

    copilot::PromptResponse reply;
    grpc::ClientContext grpc_ctx;

    grpc::Status status = stub_->ExecutePrompt(&grpc_ctx, request, &reply);

    if (status.ok()) {
        std::string debugOutput = "Reply: " + reply.reply_text() + "\n";
        
        // Loop over the parsed Protobuf actions returned by the Go backend!
        for (const auto& action : reply.actions()) {
            switch (action.action_payload_case()) {
                case copilot::Action::kCreateLayer:
                    debugOutput += " -> CreateLayer: " + action.create_layer().name() + "\n";
                    break;
                case copilot::Action::kSetProperty:
                    debugOutput += " -> SetProperty: " + action.set_property().layer_id() + " = " + std::to_string(action.set_property().value()) + "\n";
                    break;
                case copilot::Action::kAddKeyframe:
                    debugOutput += " -> AddKeyframe: " + action.add_keyframe().layer_id() + " at t=" + std::to_string(action.add_keyframe().time_seconds()) + "\n";
                    break;
                case copilot::Action::kRemoveKeyframe:
                    debugOutput += " -> RemoveKeyframe: " + action.remove_keyframe().layer_id() + "\n";
                    break;
                case copilot::Action::ACTION_PAYLOAD_NOT_SET:
                    debugOutput += " -> Unknown Action\n";
                    break;
            }
        }
        return debugOutput;
    } else {
        return "RPC failed: " + status.error_message();
    }
}
