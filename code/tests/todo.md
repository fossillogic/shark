## TODO

---

### Search Command Improvements
- [ ] Add regex support to `search`
- [ ] Add phrase-based search (exact match queries)
- [ ] Add `--regex` flag for explicit regex mode
- [ ] Add `--phrase` flag for exact string matching
- [ ] Improve performance for large directory scans
- [ ] Support combined filters (name + content + type)

---

### Output & Media Standardization
- [ ] Replace all `--json` / `--fson` flags with unified `--media` flag
- [ ] Support `--media=json`
- [ ] Support `--media=fson`
- [ ] Support `--media=text`
- [ ] Ensure ALL 30 commands respect `--media`
- [ ] Normalize output schemas across commands
- [ ] Add validation for unsupported media types
- [ ] Update documentation and examples

---

### Help System Enhancements
- [ ] Add `--ask` flag to `help` command
- [ ] Allow natural language queries:
  - [ ] `shark help show --ask "how do I filter by size?"`
- [ ] Provide contextual answers based on command flags
- [ ] Integrate examples into responses
- [ ] Add fuzzy matching for command lookup

---

### Testing & Development Improvements
- [ ] Add silent/test mode for commands (no stdout)
- [ ] Introduce internal flag (e.g., `--no-output` or `--test`)
- [ ] Separate logic from presentation layer
- [ ] Make commands return structured data internally
- [ ] Improve unit test coverage for all commands
- [ ] Add mock filesystem layer for testing
- [ ] Ensure deterministic outputs for tests

---

### Introspection Enhancements
- [ ] Expand `introspect` with richer metadata
- [ ] Include:
  - [ ] File hashes (md5/sha256)
  - [ ] Permissions/ownership
  - [ ] Extended attributes (if available)
- [ ] Add deep inspection mode (`--deep`)
- [ ] Improve structured output via `--media`
- [ ] Unify with `show` where appropriate

---

### Core Command Enhancements
- [ ] Add `--dry-run` consistency across ALL applicable commands
- [ ] Ensure `--progress` works uniformly
- [ ] Standardize `--include` / `--exclude` behavior
- [ ] Improve error messaging consistency
- [ ] Add global filtering system usable across commands
- [ ] Add consistent sorting behavior (`--sort`)

---

### Architecture & Design
- [ ] Fully separate command logic from CLI rendering
- [ ] Introduce internal data model shared across commands
- [ ] Build a unified flag parsing system
- [ ] Reduce duplication between commands (merge/move/copy)
- [ ] Add plugin/extension system (future-proofing)

---

### Documentation
- [ ] Update all command docs for `--media`
- [ ] Add advanced usage examples per command
- [ ] Document internal architecture (for contributors)
- [ ] Add “Shark vs Traditional Tools” expanded section
- [ ] Provide CLI cheat sheet

---

### Packaging & Distribution
- [ ] Package as Snapcraft app
- [ ] Package as Flatpak (pakapp)
- [ ] Provide static builds for Linux/macOS/Windows
- [ ] Add Homebrew formula
- [ ] Add installation scripts (curl | sh style optional)
- [ ] Ensure CI builds artifacts for all platforms

---

### Extras / UX Enhancements
- [ ] Expand `play` command with more games
- [ ] Add interactive modes for key commands
- [ ] Improve colorized output (`--color`)
- [ ] Add themes or output styles
- [ ] Improve CLI ergonomics (shortcuts, aliases)

---

### Future Ideas
- [ ] Add command history + replay system
- [ ] Add session awareness (stateful operations)
- [ ] Add scripting mode for chaining commands
- [ ] Introduce config file support (`shark.toml` or similar)
- [ ] Add AI-assisted suggestions (optional feature)
