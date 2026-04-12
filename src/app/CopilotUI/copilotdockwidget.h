#ifndef COPILOT_DOCK_WIDGET_H
#define COPILOT_DOCK_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

class CopilotDockWidget : public QWidget {
    Q_OBJECT

public:
    explicit CopilotDockWidget(QWidget *parent = nullptr);
    ~CopilotDockWidget() override;

private slots:
    void submitPrompt();
    void appendServerResponse(const QString& response);

private:
    QTextEdit* mChatHistory;
    QLineEdit* mPromptInput;
    QPushButton* mSubmitBtn;
};

#endif // COPILOT_DOCK_WIDGET_H
