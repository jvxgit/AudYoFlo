import 'dart:io';
import 'package:path/path.dart';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:provider/provider.dart';
import 'package:cross_file/cross_file.dart';
import 'package:desktop_drop/desktop_drop.dart';

import 'package:collection/collection.dart';

import 'package:filesystem_picker/filesystem_picker.dart';

// =======================================================================
// =======================================================================
class AudYoFloFileInputCoreWidgetNative extends StatefulWidget {
  const AudYoFloFileInputCoreWidgetNative(
      this.identT, this.textShowDrag, this.sizeIcon,
      {Key? key})
      : super(key: key);
  final JvxComponentIdentification identT;
  final String textShowDrag;
  final double sizeIcon;

  @override
  _AudYoFloFileInputCoreWidgetNativeState createState() =>
      _AudYoFloFileInputCoreWidgetNativeState();
}

class _AudYoFloFileInputCoreWidgetNativeState
    extends State<AudYoFloFileInputCoreWidgetNative>
    with
        AudYoFloMultiComponentsPropertyOnChange,
        AudYoFloPropertyOnChangeGroups {
  final List<XFile> _list = [];
  bool _dragging = false;
  AudYoFloBackendCache? theBeCache;
  late String propName;
  final String groupName = 'fileInput';
  String? folderLastTime;
  List<String>? drives;

  AudYoFloPropertyOnChangeOneGroup oneGroupTechnology =
      AudYoFloPropertyOnChangeOneGroup(['/open_filename']);

  AudYoFloOneComponentSettingsBackend mySettings =
      AudYoFloOneComponentSettingsBackend(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
              slotid: -1,
              slotsubid: -1),
          'tagFileInput',
          []);

  _AudYoFloFileInputCoreWidgetNativeState() {
    // Extract the property name
    var elm = oneGroupTechnology.propsInGroup.first;
    propName = elm.propertyDescr;

    theGroups[groupName] = oneGroupTechnology;

    createList();

    // Store the local property component
    mySettings.propsDescriptors = propsAllGroups;
  }

  @override
  AudYoFloOneComponentSettings? requestedPropertiesList(
      JvxComponentIdentification cpId) {
    if (cpId == mySettings.cpId) {
      return mySettings;
    }
    return null;
  }

  @override
  Widget innerWidget(BuildContext context) {
    // Reconstruct the properties from the latestResults struct
    var fElm = latestResult.firstWhereOrNull((str) {
      if (str is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        return (str.cpId == mySettings.cpId);
      }
      return false;
    });
    if (fElm != null) {
      if (fElm is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        if (fElm.bprops != null) {
          reconstructProperyReferences(fElm.bprops!);
        }
      }
    }

    AudYoFloPropertyOnChangeOneGroup? groupLocalWidget;
    MapEntry<String, AudYoFloPropertyOnChangeOneGroup>? groupEntryLower =
        theGroups.entries
            .firstWhereOrNull((element) => element.key == groupName);
    if (groupEntryLower != null) {
      groupLocalWidget = groupEntryLower.value;
    }

    AudYoFloPropertySingleStringBackend? propsLocal;
    if (groupLocalWidget != null) {
      propsLocal = groupLocalWidget.extractPropString(propName);
    }

    return Tooltip(
      message:
          'Select file in file dialog: long tap: open folder as used before; double-tap: open folder from root of file system',
      child: GestureDetector(
        child: DropTarget(
          onDragDone: (detail) {
            for (var elmF in detail.files) {
              String fName = elmF.path;

              // In order to work properly, we need to extend the backslashes
              fName = AudYoFloHelper.propertyStringBackSlashes(fName);
              List<String> props = [propName];
              if (propsLocal != null) {
                propsLocal.value = fName;
                if (theBeCache != null) {
                  theBeCache!.triggerSetProperties(widget.identT, props);
                }
              } else {
                print('Warning: Failed to set property ');
              }
            }
          },
          onDragEntered: (detail) {
            setState(() {
              _dragging = true;
            });
          },
          onDragExited: (detail) {
            setState(() {
              _dragging = false;
            });
          },
          child: Padding(
            padding: const EdgeInsets.all(8.0),
            child: Container(
                color:
                    _dragging ? Colors.blue.withOpacity(0.4) : Colors.black26,
                child: Center(
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Tooltip(
                        message: widget.textShowDrag,
                        child: Icon(Icons.headphones_outlined,
                            /*Icon(Icons.audio_file_outlined,*/
                            size: widget.sizeIcon),
                      ),
                    ],
                  ),
                )),
          ),
        ),
        onLongPress: () async {
          AudYoFloHelper.dbgPrint('Drop Target - Long Press');
          // We can also open a file dialog here!!

          var root = findRoot(Directory.current);
          if (folderLastTime != null) {
            root = Directory(folderLastTime!);
          }
          if (propsLocal != null) {
            runFilePicker(context, root, propsLocal, true);
          } else {
            print('Warning: Failed to set property ');
          }
        },
        onDoubleTap: () async {
          AudYoFloHelper.dbgPrint('Drop Target - Long Press');
          // We can also open a file dialog here!!
          var root = findRoot(Directory.current);
          if (propsLocal != null) {
            runFilePicker(context, root, propsLocal, false);
          } else {
            print('Warning: Failed to set property ');
          }
        },
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);

    if (configured == false) {
      // Read out the component of focus from the widget
      mySettings.cpId = widget.identT;
      resetMultiPropertySettings();
      setComponentConsider(mySettings.cpId);
    }
    return localBuild(context);
  }

  Directory findRoot(FileSystemEntity entity) {
    final Directory parent = entity.parent;
    if (parent.path == entity.path) return parent;
    return findRoot(parent);
  }

  Future<List<String>> existingDrives() async {
    List<String> lst = [];
    List<String> drivesOptions = [];
    for (int i = 65; i <= 90; i++) {
      drivesOptions.add(String.fromCharCode(i).toUpperCase() + ':');
    }

    for (var elm in drivesOptions) {
      var dir = Directory(elm);
      var ex = await dir.exists();
      if (ex) {
        lst.add(elm);
      }
    }
    return lst;
  }

  Future<void> runFilePicker(
      BuildContext context,
      Directory root,
      AudYoFloPropertySingleStringBackend propsLocal,
      bool forceDirectory) async {
    if (drives == null) {
      drives = await existingDrives();
    }

    List<FilesystemPickerShortcut> lstShortcuts = [];
    if (drives != null) {
      for (var elm in drives!) {
        lstShortcuts.add(FilesystemPickerShortcut(
            name: elm, path: Directory(elm), icon: Icons.folder));
      }

      String? result;
      String titleText = 'Select Audio File [wav, mp3, m4a]';
      if (lstShortcuts.length == 0 || forceDirectory) {
        result = await FilesystemPicker.openDialog(
            title: titleText,
            context: context,
            rootDirectory: root,
            allowedExtensions: ['.wav', '.mp3', '.m4a'],
            showGoUp: true,
            rootName: 'Root Folder',
            fileTileSelectMode: FileTileSelectMode.wholeTile);
      } else {
        result = await FilesystemPicker.openDialog(
            title: titleText,
            context: context,
            allowedExtensions: ['.wav', '.mp3', '.m4a'],
            shortcuts: lstShortcuts,
            showGoUp: true,
            rootName: 'Computer',
            fileTileSelectMode: FileTileSelectMode.wholeTile);
      }
      // FilePickerResult? result = await FilePicker.platform.pickFiles();

      if (result != null) {
        String token = result;
        folderLastTime = dirname(result);
        AudYoFloHelper.dbgPrint('Picked file <$token>');
        List<String> props = [propName];
        propsLocal.value = token;
        if (theBeCache != null) {
          theBeCache!.triggerSetProperties(widget.identT, props);
        }
      } else {
        // User canceled the picker
      }
    }
  }
}

// ========================================================================

/*

  */
