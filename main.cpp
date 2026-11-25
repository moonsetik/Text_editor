#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSaveFile>
#include <QString>
#include <QTextDocument>

class EditorWindow : public QMainWindow {
public:
    EditorWindow() : editor(new QPlainTextEdit(this))
    {
        editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        setCentralWidget(editor);
        resize(850, 580);

        auto* fileMenu = menuBar()->addMenu(tr("&Файл"));
        fileMenu->addAction(tr("&Создать"), QKeySequence::New, this, &EditorWindow::newDocument);
        fileMenu->addAction(tr("&Открыть…"), QKeySequence::Open, this, &EditorWindow::openDocument);
        fileMenu->addAction(tr("Со&хранить"), QKeySequence::Save, this, &EditorWindow::saveDocument);
        fileMenu->addAction(tr("Сохранить &как…"), QKeySequence::SaveAs, this, &EditorWindow::saveAs);
        fileMenu->addSeparator();
        fileMenu->addAction(tr("В&ыход"), QKeySequence::Quit, this, &QWidget::close);

        connect(editor->document(), &QTextDocument::modificationChanged,
                this, &QWidget::setWindowModified);
        updateTitle();
        editor->setFocus();
    }

protected:
    void closeEvent(QCloseEvent* event) override
    {
        if (confirmDiscardOrSave()) {
            event->accept();
        } else {
            event->ignore();
        }
    }

private:
    QPlainTextEdit* editor;
    QString currentPath;
    bool hasUtf8Bom = false;
    bool usesCrLf = true;

    void newDocument()
    {
        if (!confirmDiscardOrSave()) {
            return;
        }

        hasUtf8Bom = false;
        usesCrLf = true;
        setDocument({}, {});
    }

    void openDocument()
    {
        const QString path = QFileDialog::getOpenFileName(
            this, tr("Открыть файл"), currentPath, tr("Текстовые файлы (*.txt)"));
        if (path.isEmpty()) {
            return;
        }

        if (!validatePath(path) || !confirmDiscardOrSave()) {
            return;
        }

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            showFileError(tr("Не удалось открыть файл"), path, file.errorString());
            return;
        }

        QByteArray bytes = file.readAll();
        if (file.error() != QFileDevice::NoError) {
            showFileError(tr("Не удалось прочитать файл"), path, file.errorString());
            return;
        }

        const bool bom = bytes.startsWith("\xEF\xBB\xBF");
        if (bom) {
            bytes.remove(0, 3);
        }

        if (!bytes.isValidUtf8() || bytes.contains('\0')) {
            showFileError(tr("Неподдерживаемый формат текста"), path,
                          tr("Ожидается текстовый файл в кодировке UTF-8."));
            return;
        }

        hasUtf8Bom = bom;
        const qsizetype firstNewline = bytes.indexOf('\n');
        usesCrLf = firstNewline < 0 ||
                   (firstNewline > 0 && bytes.at(firstNewline - 1) == '\r');
        QString text = QString::fromUtf8(bytes);
        text.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
        text.replace(QLatin1Char('\r'), QLatin1Char('\n'));
        setDocument(text, path);
    }

    bool saveDocument()
    {
        return currentPath.isEmpty() ? saveAs() : writeDocument(currentPath);
    }

    bool saveAs()
    {
        QFileDialog dialog(this, tr("Сохранить файл"), currentPath,
                           tr("Текстовые файлы (*.txt)"));
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix(QStringLiteral("txt"));

        if (dialog.exec() != QDialog::Accepted) {
            return false;
        }

        const QString path = dialog.selectedFiles().value(0);
        return !path.isEmpty() && validatePath(path) && writeDocument(path);
    }

    bool writeDocument(const QString& path)
    {
        QString text = editor->document()->toRawText();
        text.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
        if (usesCrLf) {
            text.replace(QStringLiteral("\n"), QStringLiteral("\r\n"));
        }

        QByteArray bytes = text.toUtf8();
        if (hasUtf8Bom) {
            bytes.prepend("\xEF\xBB\xBF");
        }

        QSaveFile file(path);
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() ||
            !file.commit()) {
            showFileError(tr("Не удалось сохранить файл"), path, file.errorString());
            return false;
        }

        currentPath = path;
        editor->document()->setModified(false);
        updateTitle();
        return true;
    }

    bool confirmDiscardOrSave()
    {
        if (!editor->document()->isModified()) {
            return true;
        }

        const auto answer = QMessageBox::warning(
            this, tr("Несохранённые изменения"), tr("Сохранить изменения в документе?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);

        if (answer == QMessageBox::Save) {
            return saveDocument();
        }
        return answer == QMessageBox::Discard;
    }

    bool validatePath(const QString& path)
    {
        if (QFileInfo(path).suffix().compare(QStringLiteral("txt"), Qt::CaseInsensitive) == 0) {
            return true;
        }

        showFileError(tr("Неподдерживаемое расширение"), path,
                      tr("У файла должно быть расширение .txt."));
        return false;
    }

    void setDocument(const QString& text, const QString& path)
    {
        currentPath = path;
        editor->setPlainText(text);
        editor->document()->setModified(false);
        updateTitle();
        editor->setFocus();
    }

    void updateTitle()
    {
        const QString name = currentPath.isEmpty() ? tr("Без имени") : QFileInfo(currentPath).fileName();
        setWindowTitle(tr("%1[*] — Текстовый редактор").arg(name));
    }

    void showFileError(const QString& title, const QString& path, const QString& detail)
    {
        QMessageBox message(QMessageBox::Critical, title,
                            QStringLiteral("%1\n\n%2").arg(path, detail),
                            QMessageBox::Ok, this);
        message.setTextFormat(Qt::PlainText);
        message.exec();
    }
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    EditorWindow window;
    window.show();
    return app.exec();
}
