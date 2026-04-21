# Folio

**Folio** is a focused PDF typewriter and annotation tool for Linux, built as a
configured fork of [Xournal++](https://github.com/xournalpp/xournalpp).

---

## The problem

Linux has no good equivalent of macOS Preview's text annotation capability.

Preview lets you open any PDF — including scanned forms, contracts, or
documents with no formal form structure — and type directly on top of it.
The result is a clean, readable overlay that saves back to a standard PDF.
It requires no special form fields, no fillable widgets, no conversion step.
It just works.

On Linux, the options are poor:

- **LibreOffice Draw** can open PDFs but treats them as editable documents,
  frequently mangling layout, fonts, and graphics in the process.
- **Okular / Evince** support annotations but are focused on highlighting and
  sticky notes, not freeform typed text positioned anywhere on the page.
- **Inkscape** can open PDFs but is a vector graphics editor — the workflow is
  awkward and the output is often unreliable for multi-page documents.
- **Scribus** is a desktop publisher, not an annotation tool.
- **PDF form fillers** only work on documents that already have embedded form
  fields.

None of these replicate the simple, reliable experience of opening a PDF,
clicking anywhere, typing, and saving.

---

## Why Xournal++

Xournal++ already solves most of the hard problems:

- Solid, battle-tested PDF rendering via Poppler
- Reliable PDF export that preserves the original document as a background
- A text tool that places typed text at arbitrary positions on a page
- A well-designed undo/redo system
- Active development and a healthy codebase

What it lacks for this specific use case is focus. Xournal++ is a
full-featured handwriting and note-taking application. Its interface reflects
that: audio recording, geometry tools, notebook-style page management, LaTeX
rendering, and a toolbar dense with drawing instruments. For someone who just
wants to fill in a form or add typed notes to a scanned document, the
interface is noisy and the intended workflow is not obvious.

Folio does not replace Xournal++. It is the same engine, configured and
trimmed for a narrower purpose.

---

## What Folio changes

All changes relative to upstream Xournal++ are contained in build scripts
and configuration files. The C++ source is modified only where necessary to
wire app identity constants defined in CMakeLists.txt.

### Removed

- **Audio recording and playback** — compiled out entirely (`ENABLE_AUDIO=OFF`)
- **Setsquare and compass geometry tools** — removed from menus and toolbars
- **"New document" menu entry** — Folio is for annotating existing PDFs, not
  creating new notebooks

### Renamed / clarified

- **"Hand Tool" → "Pan (Scroll Canvas)"** — the hand tool scrolls the
  viewport; it does not move objects. The original name caused confusion with
  apps where a hand cursor means "grab and move." The object selection and
  move tool is `SELECT_OBJECT`.

### Toolbar layout

The default Folio toolbar is reorganised to foreground the tools most
relevant to PDF annotation:

- `SELECT_OBJECT` and `SELECT_RECTANGLE` are explicit toolbar buttons (not
  buried in a dropdown) so selecting and moving placed elements is obvious
- `FILL_OPACITY` sits next to the fill toggle, making shape opacity
  discoverable
- Audio and geometry items are absent

### Build identity

App name, GApplication ID, config directory, and wrapper binary name are
all defined as CMake variables in `CMakeLists.txt`. No identity strings are
hardcoded in source. To build under a different name or ID, change the four
variables at the top of `CMakeLists.txt`:

```cmake
set(APP_NAME           "Folio")
set(APP_ID             "io.github.folio")
set(APP_CONFIG_FOLDER  "folio")
set(APP_WRAPPER_BINARY "folio")
```

---

## Relationship to upstream

Folio tracks the Xournal++ `master` branch. The intent is to rebase
periodically to pick up bug fixes and PDF rendering improvements from
upstream.

The build-identity abstraction (the `APP_*` constants in `config.h.in`) is
a candidate for upstreaming as a standalone contribution, independent of any
Folio-specific values. This would allow distributions and forks to configure
app identity at build time without patching source, which is a general
improvement to the project.

---

## License

Xournal++ is licensed under the GNU General Public License version 2 or
later. Folio inherits that license. Source code is available at the Folio
repository. All modifications are documented in the git history on the
`folio` branch.
