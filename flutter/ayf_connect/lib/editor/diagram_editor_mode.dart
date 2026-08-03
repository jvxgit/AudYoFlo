/// Steuert, ob der Canvas Bearbeitung erlaubt (Nodes hinzufügen, verschieben,
/// verbinden, löschen, umbenennen) oder nur eine reine Anzeige ist, bei der
/// sich Nodes weiterhin auswählen lassen (für den Inspector), aber nicht
/// verändert werden können.
enum DiagramEditorMode { edit, readOnly }
