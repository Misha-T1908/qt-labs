#ifndef LESSONMODEL_H
#define LESSONMODEL_H

#include <QString>
#include <QStringList>

class LessonModel
{
public:
    void setText(const QString &text);
    void reset();
    void stepForward();
    void stepBack();

    QString previousLine() const;
    QString currentLine() const;
    QString typedPart() const;
    QString remainingPart() const;
    QChar expectedChar() const;
    bool isFinished() const;
    int lineIndex() const;
    int charIndex() const;

private:
    int safeCharIndex() const;

    QString fullText;
    QStringList lines;
    int currentLineIndex = 0;
    int currentCharIndex = 0;
};

#endif
