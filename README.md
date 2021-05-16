# The Qt Client Library

The Qt Client Library encapsulates the WebSocket interface to the QuickHub core and provides a variety of objects with Qt interfaces that can be instantiated and accessed directly from QML. These interfaces allow modern cross-platform user interfaces to communicate directly with QuickHub resources and devices. Simple scenarios can thus be implemented entirely in QML and JavaScript, without the need for C++!

Thanks to the bidirectional WebSocket interface, Qt's own signal/slot concept and PropertyBindings, server-side changes to data are passed directly to the UI controls. This means that the actual implementation effort for interfaces and middleware is reduced to a minimum.


# Installation and import

The client library can be included in a Qt project in different ways.

- As a subproject via a *.pri file. The code is then compiled directly into the project.
- As a central QML plugin, which can be easily included via a QML import.

The following Qt modules are used:

- QtQml
- QtQuick
- QtWebSockets
- The library has no dependencies to thirdparty libraries other than the Qt modules mentioned.

 ###  Import via a sub-project (QHCliendModule.pri)

For this purpose the subproject must be included in the project file (.pro) via an include statement
```
include(ClientLib/QHClientModule.pri)
```
and initialized in the main.cpp before loading the main.qml.
```
include "InitCloudModels.h"
InitCloudModels::registerTypes("QuickHub");
```
From this moment the module can be easily included in a QML file
```
import QuickHub 1.0
```

### Import as QML module

Alternatively you can compile the project file of the QHClientLib directly as a library. As output the compiler generates a library (.so, .dylib or .dll - depending on the architecture) and a qmldir file. These two files must be placed in a common folder in one of the QML search paths. The folder must have the same name as the plugin. This is "CloudModels" by default.

The default search path, which is valid for an entire Qt installation, is usually located at:
```
<QtFolder>/<Version>/<Compiler>/qml/CloudModels.
```
On my computer, the folder is located at.
```
~/Qt/5.14.1/clang_64/qml
```
# Usage

## Login and connection establishment

The login, as well as everything that has to do with connection and user management, is controlled by the CloudModel.cpp class. This class is exposed in QML as a singleton object under the name "QuickHub".

If you set the property ```serverURL``` on the QuickHub object, it will try to establish a WebSocket connection to the corresponding QuickHub server. Once the property ```state```has the value ```QuickHub.connected```, ```QuickHub.login(user, password)``` can be used to establish the connection. The following code example shows a typical snippet with automatic reconnect.
```qml
import QuickHub 1.0 
 
// connect to server when Component has completed
Component.onCompleted: CloudSession.serverURL = "wss://my-quickhub-instance.io"
 
Timer
{
    id: reconnectTimer
    interval: 1000 // try to reconnect every second
    onTriggered: CloudSession.reconnectServer()
}
 
Connections
{
    target: CloudSession
    onStateChanged:
    {
        if(QuickHub.state == QuickHub.STATE_Connected)
        {
            // try to login
            QuickHub.login(qh_user,qh_password);
            reconnectTimer.running = false;
        }
 
        if(QuickHub.state == QuickHub.STATE_Disconnected)
        {
            // start reconnect timer
            reconnectTimer.start()
        }
    }
}
```
For a deeper understanding of how QAbstractItemModels implemented in C++ interact with QML, the following links can be recommended. However, this knowledge is not absolutely necessary for working with QuickHub:

- https://doc.qt.io/qt-5/qtquick-modelviewsdata-cppmodels.html#qabstractitemmodel-subclass
- https://doc.qt.io/qt-5/model-view-programming.html#model-subclassing-reference

## Services

A service in QuickHub describes a set of RPCs that can be provided by a plugin at a specific address. Via QML you can communicate directly with a service using the ServiceModel object. Services are RPCs that are provided by server services and do not need to be called at first.

```qml
import CloudModels 1.0
 
ServiceModel
{ 
    id: service
    service: "magicService" // Adresse bzw. Name des Services
}
 
Button
{
    text: "do something"
    onClicked: service.call("doIt", {"paramA":"foo", "paramB":"bar"}, callback) 
}
 
function callback(data)
{
    console.log(data.returnVal);  
}
```

## Ressources

QuickHub works internally with a modular resource concept. This means that any data can be provided by server-side plugins in the form of fixed structures. The special thing about this is that these resources synchronize in real time among all clients without having to write any glue code. This is made possible by Qt's own signal/slot concept and QProperties.

Content, semantics of access functions, and persistence of data are defined by the plugins, QuickHub provides the complete infrastructure.

Regarding data structures, QuickHub currently supports

- Generic lists (in the form of a QAbstractListModel derivative)
- Generic objects (in the form of a QQmlPropertyMap derivative).

Each resource is uniquely determined by type and address. Accordingly, when instantiating a resource, a specifier must be specified that determines which data is to be loaded.

Beside the pure display, i.e. the representation of data, the models bring also from QML via JavaScript callable modifiers. These are functions that allow the direct insertion, deletion and editing of data fields. Plugins do not have to implement these modifiers. Besides this, plugins can decide that only users with appropriate authorization can use these modifiers.

##### home/* and public/* resources

Resources that start with ```home/<resourceName>``` or ```public/<resourceName>```are a special case, as they are provided by the QuickHub core directly and not by plugins. These resources are always usable, exist implicitly, and can be filled with arbitrary data, which is then synchronized among clients accordingly. The only constraint: the structure of the data must not change for lists within a resource. The first entry in the list determines which fields exist within the whole list.

These resources are extremely useful for storing user settings, synchronizing simple lists or persisting states of UI inputs without having to write extra C++ code.

Note that ```home/*``` resources are individual for each user and ```public/*``` resources can be seen and edited by all users.

### SynchronizedListModel

The SynchronizedListModel encapsulates access to lists. Since SynchronizedListModel implements the QAbstractListModel interface, which is very common in Qt, it can interact directly with the components provided by Qt (e.g. ListView, Repeater, TableView) without any further intervention.

The following example shows the implementation of a simple ToDo list using a QuickHub native private/* list resource. It does not require any other server-side plugins to be installed.

```qml
// For reasons of clarity, all design elements (layout and size specifications) have been removed.
 
include QuickHub 1.0
 
SynchronizedListModel
{
    id: toDoListModel
    resource: "private/toDoList" // a resource generated by QuickHub, visible only to the currently logged in user
}
 
 
ListView {
   model: toDoListModel
   delegate:
   Item {
      Column {
         Text {
            text: toDoName // Name of the ToDo, is resolved by the ListView via the model.
         }        
         Text {
            text: toDoDescription // Description of the toDo of the ToDo list, is resolved by the ListView via the model.
         }
      }
   }
}
 
Column {
    TextField {
       id: nameField
       placeHolderText: "enter name"
    }
 
    TextField {
       id: descriptionField
       placeHolderText: "enter description"
    }
 
    Button {
       text: "Insert ToDo"
       // insert an entry 
       onClicked: toDoListModel.append({"toDoName":nameField.text, "toDoDescription": descriptionField.text})
    }
}
```

### SynchronizedObjectModel

The ```SynchronizedObjectModel``` follows a similar concept as the ```SynchronizedListModel```. The only difference is that it is a synchronized key-value store that synchronizes the individual values with the UI components via property bindings. To get a feel for how it works, it's worth taking a look at the [QQmlPropertyMap](https://doc.qt.io/qt-5/qqmlpropertymap.html) documentation.

```qml
include CloudModels 1.0
 
SynchronizedObjectModel {
    id: userSettingsModel
    resource: "private/userSettings"
}
 
Column {
    anchors.centerIn: parent
    CheckBox {
        text:"A"
        onCheckedChanged: userSettingsModel.checkedA = checked
        // false is the default value, at the very first call. From that moment on, the state of the check boxes is synchronized across 
        // all instances of the client for the respective user in real time.
        checked: userSettingsModel.initialized && userSettingsModel.checkedA !== undefined  ? userSettingsModel.checkedA : false
 
    }
 
    CheckBox {
        text:"B"
        onCheckedChanged: userSettingsModel.checkedB = checked
        checked: userSettingsModel.initialized && userSettingsModel.checkedB !== undefined  ? userSettingsModel.checkedB : false
    }
}


