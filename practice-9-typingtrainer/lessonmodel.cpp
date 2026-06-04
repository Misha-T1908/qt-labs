#include "lessonmodel.h"

void LessonModel::setText(const QString &text)
{
    fullText = text;
    lines = fullText.replace("\r\n", "\n").replace('\r', '\n').split('\n');
    if (lines.isEmpty()) {
        lines.append("");
    }
    reset();
}

void LessonModel::reset()
{
    currentLineIndex = 0;
    currentCharIndex = 0;
}

void LessonModel::stepForward()
{
    if (lines.isEmpty() || currentLineIndex >= lines.size()) {
        return;
    }

    const int length = lines.at(currentLineIndex).size();
    if (currentCharIndex < length) {
        ++currentCharIndex;
        return;
    }

    if (currentLineIndex + 1 < lines.size()) {
        ++currentLineIndex;
        currentCharIndex = 0;
    }
}

void LessonModel::stepBack()
{
    if (lines.isEmpty()) {
        return;
    }

    if (currentCharIndex > 0) {
        --currentCharIndex;
        return;
    }

    if (currentLineIndex > 0) {
        --currentLineIndex;
        currentCharIndex = lines.at(currentLineIndex).size();
    }
}

QString LessonModel::previousLine() const
{
    if (currentLineIndex <= 0 || currentLineIndex - 1 >= lines.size()) {
        return "";
    }
    return lines.at(currentLineIndex - 1);
}

QString LessonModel::currentLine() const
{
    if (lines.isEmpty() || currentLineIndex < 0 || currentLineIndex >= lines.size()) {
        return "";
    }
    return lines.at(currentLineIndex);
}

QString LessonModel::typedPart() const
{
    return currentLine().left(safeCharIndex());
}

QString LessonModel::remainingPart() const
{
    return currentLine().mid(safeCharIndex());
}

QChar LessonModel::expectedChar() const
{
    const QString line = currentLine();
    const int index = safeCharIndex();
    if (index >= 0 && index < line.size()) {
        return line.at(index);
    }
    return QChar('\n');
}

bool LessonModel::isFinished() const
{
    if (lines.isEmpty()) {
        return true;
    }
    return currentLineIndex == lines.size() - 1 && currentCharIndex >= lines.last().size();
}

int LessonModel::lineIndex() const
{
    return currentLineIndex;
}

int LessonModel::charIndex() const
{
    return currentCharIndex;
}

int LessonModel::safeCharIndex() const
{
    const int length = currentLine().size();
    if (currentCharIndex < 0) {
        return 0;
    }
    if (currentCharIndex > length) {
        return length;
    }
    return currentCharIndex;
}
