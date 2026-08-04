/// Steuert, welche Interaktionen der Canvas erlaubt:
/// - [edit]: volle Bearbeitung (Nodes hinzufügen, verschieben, verbinden,
///   löschen, umbenennen).
/// - [moveOnly]: Nodes lassen sich weiterhin auswählen und per Drag
///   verschieben (Positionsänderung), aber nicht hinzufügen, löschen,
///   umbenennen oder (dis-)connecten.
/// - [readOnly]: reine Anzeige — Nodes bleiben auswählbar (für den
///   Inspector), aber nichts, auch keine Position, kann verändert werden.
enum DiagramEditorMode { edit, moveOnly, readOnly }
