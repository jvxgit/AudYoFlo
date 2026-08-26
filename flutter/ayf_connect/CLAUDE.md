# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

ConnectorFlo is a Flutter desktop app for building data-flow diagrams: draggable node boxes connected via typed input/output ports, built on top of the `fl_nodes` package. Currently UI-only — no backend/network integration exists yet, and none is planned until the UI is further along. Only Windows/macOS/Linux desktop targets are configured (no Android/iOS/web platform folders).

## Commands

```
flutter analyze                                 # static analysis, must be clean before considering work done
flutter test                                     # run the full test suite
flutter test test/editor/fl_nodes_adapter_test.dart   # run a single test file
flutter test --plain-name "some test name"       # run a single test by name
flutter run -d windows                           # run the app (also: -d macos, -d linux)
flutter build windows --debug                    # debug build, exe at build\windows\x64\runner\Debug\connectorflo.exe
flutter build windows --release                  # release build (debug-only overlays disappear, see below)
```

## Architecture

### Two controllers, one direction of truth

There are two separate stateful controllers in play, and understanding which owns what is the key to working in this codebase:

- **`FlNodeEditorController`** (from `fl_nodes`, created inside `FlNodesAdapter`) is the live interaction engine. It owns node positions, selection, and link topology while the user drags/connects/deletes things on screen — all of `fl_nodes`' built-in gesture handling operates directly on it.
- **`DiagramController`** (`lib/state/diagram_controller.dart`, plain `ChangeNotifier`, provided app-wide via `provider`'s `ChangeNotifierProvider` in `main.dart`) is our own framework-independent, JSON-serializable mirror of the diagram (`lib/models/`: `Diagram`, `DiagramNode`, `DiagramEdge`, `PortDefinition`, `PortType`). It never talks to `fl_nodes` types directly.

**`FlNodesAdapter`** (`lib/editor/fl_nodes_adapter.dart`) is the bridge, and change generally flows one way: it listens to `flController.eventBus.events` and translates each event (add/remove node, add/remove link, drag, selection) into a call on `DiagramController`. UI widgets that need to react to diagram state (`NodeInspector`, `DiagramNodeHeader`) read `DiagramController` via `context.watch<DiagramController>()`, not the fl_nodes controller.

The exception is `FlNodesAdapter.loadDiagram(Diagram)`, which goes the other way (external `Diagram` → canvas), for programmatically populating the canvas (e.g. for read-only display). Because `flController.addNode()` always assigns its own new id (no way to pass one in), `loadDiagram` keeps an internal id-remap while rebuilding, so node/edge ids after a load differ from the ids in the `Diagram` that was passed in — only structure/positions/types/connections are preserved.

### Node types are data, registered twice

A "node type" (Source/Transform/Sink etc.) is described once as data (`NodeTypeDefinition`/`PortSpec` in `lib/editor/node_type_definition.dart`, populated in `lib/editor/default_node_types.dart`) and consumed in two places: `FlNodesAdapter` turns each into an `FlNodePrototype` (registered with `flController.registerNodePrototype`), and `NodePalette` (`lib/widgets/node_palette.dart`) lists them for the user to click-to-add. Adding a new node type means editing `default_node_types.dart` only — no change needed in the adapter or palette.

`PortType.any` is defined in the model but not used by the current catalog — see the comment in `fl_nodes_adapter.dart` above `_buildInputPort`/`_buildOutputPort` for why (`fl_nodes` checks port compatibility via Dart generic type identity, which is asymmetric for `dynamic` compared to this model's `PortType.isCompatibleWith`).

### Edit / move-only / read-only canvas

`CanvasPage(mode: DiagramEditorMode.edit | .moveOnly | .readOnly)` (`lib/editor/diagram_editor_mode.dart`) toggles between the normal editable canvas, a display that still allows repositioning nodes, and a fully static display. `fl_nodes` has **no config flag** to disable node-drag/link-drag gestures — they're unconditionally wired into its default node widget. The two restricted modes therefore swap in `RestrictedNode` (`lib/widgets/restricted_node.dart`) via `FlNodeEditorWidget.nodeBuilder`, which **fully replaces** fl_nodes' default node widget (including all its gesture handling) rather than configuring it — `RestrictedNode` reimplements the visual layout from scratch (header via the shared `DiagramNodeHeader`, ports, per-instance `groupColor`/port labels from `DiagramController`) with only a plain `onTap` for selection and, gated behind its `movable` constructor flag, a pan gesture for `controller.dragSelection` — no connect/disconnect handling in either mode. It must preserve `node.key`/`port.key` (`GlobalKey`s) exactly as the default widget does, since link-line rendering depends on them to find port positions.

`movable: true` (move-only) always narrows the selection to just the dragged node at pan-start, rather than mirroring fl_nodes' own `DefaultNodeWidget` behavior of keeping an existing multi-selection: fl_nodes' background-drag area-selection (`FlNodeEditorConfig.enableAreaSelection`, on by default in every mode) can select nodes without ever routing an event through this widget, so without that forced narrowing, a drag starting on a node that happens to already be selected from an earlier area-selection would drag the whole old selection along instead of just the grabbed node.

### fl_nodes gotchas worth knowing before touching `fl_nodes_adapter.dart`

Found by reading `fl_nodes` 0.5.0+1 source directly; still true as of that version:

- `FlNodeEditorController.addLink()`'s `FromTo` record has misleading field names: actual semantics are `from`=output node id, `to`=output **port** id, `fromPort`=input **node** id, `toPort`=input port id. See the comment in `_mirrorAddLink`.
- `FlNodeSelectionEvent` with `type=deselect` carries the *previously* selected node ids (about to be cleared), not the new selection. `clearSelection()` always clears everything; there's no partial deselect.
- `FlSelectionEventType` is not exported from the public `fl_nodes.dart` barrel despite being the type of an exported field — compared via `.name` (string) in `_mirrorSelection` instead of importing from `src/`.
- `FlNodeEditorConfig` defaults `autoBuildGraph`/`autoRunGraph` to `true` with 5s delays, which schedules internal timers even though this app never uses the graph-execution feature. `FlNodesAdapter` explicitly passes `FlNodeEditorConfig(autoBuildGraph: false, autoRunGraph: false)` — without this, `testWidgets` tests fail on teardown ("Timer is still pending").
- `snapToGridSize` defaults to `64.0` with snapping on — any test asserting exact node positions needs grid-aligned offsets (see the `dragSelection`/`loadDiagram` tests in `fl_nodes_adapter_test.dart`).
- The green dot + stats overlay (offset/zoom/node count/...) visible on the canvas is `fl_nodes`' own `kDebugMode`-gated debug overlay, not app code — it disappears in `flutter build --release`.
- `FlNodeDataModel.builtStyle`/`builtHeaderStyle` are normally initialized by fl_nodes' internal `DefaultNodeWidget.initState()` before `nodeBuilder` is ever invoked. Testing `RestrictedNode` standalone (outside a full `FlNodeEditorWidget` tree) requires priming them manually first — see `_primeStyles` in `test/widgets/restricted_node_test.dart`.

### Widget tests needing `DiagramController`

Any widget under test that uses `context.watch<DiagramController>()`/`context.read<DiagramController>()` (`NodeInspector`, `DiagramNodeHeader`, and anything embedding them, including `RestrictedNode`) needs a `ChangeNotifierProvider<DiagramController>` ancestor in the test's widget tree, or it throws `ProviderNotFoundException`. See the `_wrap`/`_wrapNode` helpers in the relevant test files for the pattern.
