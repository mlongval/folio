# CLAUDE.md — Folio development context

Folio is a focused PDF typewriter and annotation tool, built as a configured
fork of Xournal++. See `FOLIO.md` for the user-facing rationale.

---

## Build

```sh
cd build && ninja          # incremental build
ninja install              # install to ~/.local
```

After changing `toolbar.ini.in`, also copy the generated file:
```sh
cp build/resources-templates/toolbar.ini ~/.local/share/folio/ui/toolbar.ini
```

The active toolbar preset is set in `~/.config/folio/settings.xml`
(`selectedToolbar = "Folio"`).

---

## Fork identity

All app-identity strings are CMake variables in `CMakeLists.txt`:

```cmake
set(APP_NAME           "Folio")
set(APP_ID             "io.github.folio")
set(APP_CONFIG_FOLDER  "folio")
set(APP_WRAPPER_BINARY "folio")
```

`PROJECT_NAME` (lowercase "folio") is available as a C++ `constexpr` via
`config.h`. Use it instead of hardcoding "xournalpp" anywhere in paths.
`XournalMain.cpp::findResourcePath()` was fixed to use `PROJECT_NAME` for
this reason.

---

## Key architectural patterns

### Action / ActionProperties
All toolbar and menu actions go through `ActionDatabase`. To add a new
stateful toolbar toggle:
1. Add enum value to `src/core/enums/Action.enum.h`
2. Add the string name in `src/core/enums/generated/Action.NameMap.generated.h`
   (same ordinal position — the generated file is manually kept in sync here)
3. Add `ActionProperties<Action::YOUR_ACTION>` specialisation in
   `src/core/control/actions/ActionProperties.h`
4. Register a toolbar item in `ToolMenuHandler::initToolItems()` using one of:
   - `emplaceStockItemTgl` — GTK stock icon, bool toggle
   - `emplaceCustomItemTgl` — custom loaded icon, bool toggle
   - `emplaceStockItem` / `emplaceCustomItem` — non-toggle button
5. Add the item name to `resources-templates/toolbar.ini.in`

### Text element rendering
Cairo draws text at `(x, y)` with `cairo_translate(x,y)` then
`cairo_rotate(rotation)`. The pivot is the **top-left corner**, not the
centre. AABB of a rotated rectangle must compute all 4 corners then
take min/max — see `Text::updateSnapping()` in `model/Text.cpp`.

### Editing rotated text (TextEditor clone pattern)
`TextEditor` works on a **clone** of the text element (`textElement`);
the original stays in the layer with `inEditing = true` (rendered
transparent/skipped by `TextView`). On finalize, the clone replaces the
original.

For rotated text, the clone is **un-rotated at edit start** (pivot at
anchor = no position change) and **re-rotated at finalize**. Members:
- `TextEditor::savedRotation` — stashed angle
- `initializeEditionAt()` — zeroes the clone's rotation
- `finalizeEdition()` — restores `savedRotation` before inserting clone

### Bold / Italic / Underline
- **Bold / Italic** are encoded in the Pango font name string
  (e.g., `"Sans Bold Italic"`). Toggle by finding/inserting `" Bold"` or
  `" Italic"` in `XojFont::name`.
- **Underline** is a `bool` field on `Text`, serialized to `.xopp` XML as
  `underline="1"`, applied via `PangoAttrList` in `Text::createPangoLayout()`.
- `Control::syncTextFormattingState()` updates the three action states
  whenever the active text element changes.

### Font action state
`Control::setFontSelected(font)` sets `Action::FONT` state without
triggering callbacks (used to sync the font-picker button when a text
element is selected for editing). `Control::fontChanged(font)` is the
callback path (updates settings, applies to selection/editor).

### Save / Load pipeline for text attributes
- `SaveHandler.cpp` — writes XML attributes for text nodes
- `XmlAttrs.h` — defines attribute name constants (e.g. `ROTATION_STR`,
  `UNDERLINE_STR`)
- `XmlParser.cpp` — parses text tag, calls `builder.addText(...)`
- `DocumentBuilderInterface.h` — `addText()` virtual signature
- `LoadHandler.cpp` — concrete implementation, constructs `Text` objects

When adding a new text attribute: touch all five files.

---

## Open bugs / known limitations

### Rotated text editing visual artifact
The un-rotate-on-edit approach is implemented. Needs testing to confirm
there are no remaining artifacts (overlapping characters, wrong frame
position). Test: create text → rotate → double-click to edit → confirm
text appears horizontal → commit → confirm rotation restored.

Relevant commits: `de07b7f37` (un-rotate refactor), preceding AABB fixes.

---

## Folio-specific toolbar items

Items added beyond upstream Xournal++:
- `TEXT_BOLD`, `TEXT_ITALIC`, `TEXT_UNDERLINE` — text formatting toggles

Items deliberately absent from the Folio preset (still registered, just
not in the Folio toolbar):
- `MATH_TEX`, `DEFAULT_TOOL`, `HAND`, `VERTICAL_SPACE`
