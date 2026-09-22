/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../PresentationLayer/mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onLoadImage",
        "",
        "onSaveImage",
        "onClear",
        "onUndo",
        "onRedo",
        "onFilterSelected",
        "index",
        "onBrightnessPreview",
        "value",
        "onContrastPreview",
        "onSaturationPreview",
        "onHuePreview",
        "onFreeRotatePreview",
        "angle",
        "onApplyBrightness",
        "onApplyContrast",
        "onApplySaturation",
        "onApplyHue",
        "onApplyFreeRotate",
        "onToggleFreeCrop",
        "checked",
        "onApplyFreeCrop",
        "onCancelFreeCrop",
        "onOpenResizeDialog",
        "handleWorkspaceImageDropped",
        "path",
        "onImageLoaded",
        "onImageProcessed",
        "onHistoryChanged",
        "onError",
        "message",
        "onFocusChanged",
        "onImageActivated",
        "filePath",
        "onStickerActivated",
        "onRemoveSelectedSticker",
        "onMoveUpSticker",
        "onMoveDownSticker",
        "onApplyFilterToSticker",
        "filterIndex",
        "onToggleSelectionTool",
        "onSelectionModeChanged",
        "onApplyFilterToRegion",
        "onToggleRotateView",
        "onZoomSliderChanged",
        "onViewZoomChanged",
        "percent"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onLoadImage'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveImage'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClear'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUndo'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRedo'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFilterSelected'
        QtMocHelpers::SlotData<void(int)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'onBrightnessPreview'
        QtMocHelpers::SlotData<void(int)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Slot 'onContrastPreview'
        QtMocHelpers::SlotData<void(int)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Slot 'onSaturationPreview'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Slot 'onHuePreview'
        QtMocHelpers::SlotData<void(int)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Slot 'onFreeRotatePreview'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onApplyBrightness'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplyContrast'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplySaturation'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplyHue'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplyFreeRotate'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleFreeCrop'
        QtMocHelpers::SlotData<void(bool)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 22 },
        }}),
        // Slot 'onApplyFreeCrop'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCancelFreeCrop'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenResizeDialog'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleWorkspaceImageDropped'
        QtMocHelpers::SlotData<void(const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 27 },
        }}),
        // Slot 'onImageLoaded'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImageProcessed'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onHistoryChanged'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onError'
        QtMocHelpers::SlotData<void(const QString &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 32 },
        }}),
        // Slot 'onFocusChanged'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImageActivated'
        QtMocHelpers::SlotData<void(const QString &)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 35 },
        }}),
        // Slot 'onStickerActivated'
        QtMocHelpers::SlotData<void(const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 35 },
        }}),
        // Slot 'onRemoveSelectedSticker'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMoveUpSticker'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMoveDownSticker'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplyFilterToSticker'
        QtMocHelpers::SlotData<void(int)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 41 },
        }}),
        // Slot 'onToggleSelectionTool'
        QtMocHelpers::SlotData<void(bool)>(42, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 22 },
        }}),
        // Slot 'onSelectionModeChanged'
        QtMocHelpers::SlotData<void(int)>(43, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'onApplyFilterToRegion'
        QtMocHelpers::SlotData<void(int)>(44, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 41 },
        }}),
        // Slot 'onToggleRotateView'
        QtMocHelpers::SlotData<void(bool)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 22 },
        }}),
        // Slot 'onZoomSliderChanged'
        QtMocHelpers::SlotData<void(int)>(46, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Slot 'onViewZoomChanged'
        QtMocHelpers::SlotData<void(double)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 48 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onLoadImage(); break;
        case 1: _t->onSaveImage(); break;
        case 2: _t->onClear(); break;
        case 3: _t->onUndo(); break;
        case 4: _t->onRedo(); break;
        case 5: _t->onFilterSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onBrightnessPreview((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->onContrastPreview((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->onSaturationPreview((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onHuePreview((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->onFreeRotatePreview((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->onApplyBrightness(); break;
        case 12: _t->onApplyContrast(); break;
        case 13: _t->onApplySaturation(); break;
        case 14: _t->onApplyHue(); break;
        case 15: _t->onApplyFreeRotate(); break;
        case 16: _t->onToggleFreeCrop((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 17: _t->onApplyFreeCrop(); break;
        case 18: _t->onCancelFreeCrop(); break;
        case 19: _t->onOpenResizeDialog(); break;
        case 20: _t->handleWorkspaceImageDropped((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->onImageLoaded(); break;
        case 22: _t->onImageProcessed(); break;
        case 23: _t->onHistoryChanged(); break;
        case 24: _t->onError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 25: _t->onFocusChanged(); break;
        case 26: _t->onImageActivated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->onStickerActivated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 28: _t->onRemoveSelectedSticker(); break;
        case 29: _t->onMoveUpSticker(); break;
        case 30: _t->onMoveDownSticker(); break;
        case 31: _t->onApplyFilterToSticker((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 32: _t->onToggleSelectionTool((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 33: _t->onSelectionModeChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 34: _t->onApplyFilterToRegion((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 35: _t->onToggleRotateView((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 36: _t->onZoomSliderChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 37: _t->onViewZoomChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 38)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 38;
    }
    return _id;
}
QT_WARNING_POP
