Scriptname PAPER_SKSEFunctions Hidden

; Resources
bool Function ResourceExists(String asResourcePath) global native
String[] Function GetInstalledResources(String[] asStrings) global native

; ActorBase
ColorForm[] Function GetWarpaintColors(ActorBase akActorBase) global native

; Helper functions for filtering arguments of Inventory Events
int[] Function GetInventoryEventFilterIndices(Form[] akEventItems, Form akFilter, int[] aiIndices=None) global native
Form[] Function ApplyInventoryEventFilterToForms(int[] aiIndicesToKeep, Form[] akFormArray) global native
int[] Function ApplyInventoryEventFilterToInts(int[] aiIndicesToKeep, int[] aiIntArray) global native
ObjectReference[] Function ApplyInventoryEventFilterToObjs(int[] aiIndicesToKeep, ObjectReference[] akObjArray) global native

; Other
int[] Function GetPaperVersion() global native