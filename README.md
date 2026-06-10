# 91 Dub v5

A fork of the great [91 Dub v4.0](https://github.com/orviwan/91-dub-4.0) watchface by Orviwan, rebuilt for the Pebble Time 2 (Emery).

## What's New

- Built targeting the PT2 platform
- Optimized for the new PT2 display resolution with improved digit and icon sizing. Looks better than auto-scaling 91 Dub v4
- Redesigned settings page, no longer dependent on external web site
- Preserves all v4.0 customizability, including color sets and themes

## Building

Requires the Pebble/Rebble SDK toolchain.

```bash
npm install
pebble build
pebble install --phone <device>
```

## Development

### Local agent skills

Pebble's pebble-watchface skill can be made available to local agents through a git submodule:

```bash
git submodule update --init
```

To pull the latest changes from the submodule's remote:

```bash
git submodule update --remote --merge
```

Symlink the pebble-watchface skill into the main repo:

```bash
ln -sf ../../pebble-watchface-agent-skill/.claude/skills/pebble-watchface .agents/skills/pebble-watchface
```

## License

MIT-style license. See [LICENSE](LICENSE) for details.
