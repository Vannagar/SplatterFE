#include "copilotdockwidget.h"
#include "../../copilot/CopilotClient.h"
#include <QMetaObject>
#include <QVariant>
#include <thread>
#include <memory>
#include <QDebug>

CopilotDockWidget::CopilotDockWidget(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    
    mChatHistory = new QTextEdit(this);
    mChatHistory->setReadOnly(true);
    mChatHistory->setPlaceholderText("Splatter.AI Agent initialized...\nType a prompt to begin.");
    
    mPromptInput = new QLineEdit(this);
    mPromptInput->setPlaceholderText("e.g. Set the Scene, add a red circle at 100, 100");
    
    mSubmitBtn = new QPushButton("Submit", this);
    
    auto inputLayout = new QHBoxLayout();
    inputLayout->addWidget(mPromptInput);
    inputLayout->addWidget(mSubmitBtn);
    
    layout->addWidget(mChatHistory);
    layout->addLayout(inputLayout);
    
    connect(mSubmitBtn, &QPushButton::clicked, this, &CopilotDockWidget::submitPrompt);
    connect(mPromptInput, &QLineEdit::returnPressed, this, &CopilotDockWidget::submitPrompt);
}

CopilotDockWidget::~CopilotDockWidget() {}

void CopilotDockWidget::submitPrompt()
{
    QString prompt = mPromptInput->text();
    if (prompt.isEmpty()) return;
    
    mChatHistory->append("<b>User:</b> " + prompt);
    mPromptInput->clear();
    mPromptInput->setEnabled(false);
    mSubmitBtn->setEnabled(false);
    
    std::string promptStr = prompt.toStdString();
    
    // Run the gRPC call asynchronously
    std::thread([this, promptStr]() {
        try {
            std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
            CopilotClient client(channel);
            
            std::string rawResponse = client.ExecutePrompt(promptStr);
            QString response = QString::fromStdString(rawResponse);
            
            QMetaObject::invokeMethod(this, "appendServerResponse",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, response));
        } catch (...) {
            QMetaObject::invokeMethod(this, "appendServerResponse",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, "Error connecting to SplatterBE"));
        }
    }).detach();
}

void CopilotDockWidget::appendServerResponse(const QString& response)
{
    mChatHistory->append("<b>Copilot:</b> " + response);
    mPromptInput->setEnabled(true);
    mSubmitBtn->setEnabled(true);
    mPromptInput->setFocus();
}
