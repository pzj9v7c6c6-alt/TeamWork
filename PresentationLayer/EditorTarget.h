#pragma once

#include <QObject>

class StickerItem;

/**
 * @brief Centralized editor focus target.
 *
 * Tracks whether the user is currently operating on the base image
 * or a specific sticker.  All filter, adjustment, undo/redo, and
 * delete operations query this object to decide their target.
 */
class EditorTarget : public QObject {
    Q_OBJECT

public:
    enum Kind { BaseImage, Sticker };

    explicit EditorTarget(QObject* parent = nullptr)
        : QObject(parent) {}

    Kind kind() const { return m_kind; }
    StickerItem* sticker() const { return m_sticker; }
    bool isBaseImage() const { return m_kind == BaseImage; }
    bool isSticker()   const { return m_kind == Sticker && m_sticker != nullptr; }

    void focusBaseImage() {
        if (m_kind == BaseImage && m_sticker == nullptr) return;
        m_kind    = BaseImage;
        m_sticker = nullptr;
        emit focusChanged();
    }

    void focusSticker(StickerItem* s) {
        if (m_kind == Sticker && m_sticker == s) return;
        m_kind    = Sticker;
        m_sticker = s;
        emit focusChanged();
    }

    /// Called when a sticker is deleted — clear focus if it was the target.
    void stickerRemoved(StickerItem* s) {
        if (m_sticker == s) {
            m_kind    = BaseImage;
            m_sticker = nullptr;
            emit focusChanged();
        }
    }

signals:
    void focusChanged();

private:
    Kind         m_kind    = BaseImage;
    StickerItem* m_sticker = nullptr;
};
