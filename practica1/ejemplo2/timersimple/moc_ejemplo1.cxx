/****************************************************************************
** Meta object code from reading C++ file 'ejemplo1.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ejemplo1.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ejemplo1.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ejemplo1[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      21,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ejemplo1[] = {
    "ejemplo1\0\0contador()\0pararContador()\0"
};

void ejemplo1::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ejemplo1 *_t = static_cast<ejemplo1 *>(_o);
        switch (_id) {
        case 0: _t->contador(); break;
        case 1: _t->pararContador(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ejemplo1::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ejemplo1::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ejemplo1,
      qt_meta_data_ejemplo1, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ejemplo1::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ejemplo1::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ejemplo1::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ejemplo1))
        return static_cast<void*>(const_cast< ejemplo1*>(this));
    if (!strcmp(_clname, "Ui_Counter"))
        return static_cast< Ui_Counter*>(const_cast< ejemplo1*>(this));
    return QWidget::qt_metacast(_clname);
}

int ejemplo1::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
