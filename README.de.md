# 91 Dub CGM (dubv4cgm)

[English](README.md) · **Deutsch**

Das klassische **91 Dub**-Watchface für die **Pebble Time 2** – mit live **CGM**-Wert aus Nightscout oder iPump-mini, Wetter, Gesundheitsdaten und der RGB-Beleuchtung.

**Status:** im Pebble Appstore · Daten aus Nightscout oder [iPump-mini](https://github.com/sgitaize/iPump-mini) · wird aktiv gepflegt

## So kommst du dran

| | |
|---|---|
| **Pebble Appstore** | [91 Dub CGM](https://apps.rePebble.com/49c4f560ff4141548025f29f) – über die Pebble-App auf dem Handy installieren |
| **Releases** | [`.pbw` auf GitHub](https://github.com/sgitaize/dubv4cgm/releases/latest) zum Seitenladen |
| **Einstellungen** | Zahnrad in der Pebble-App → [Einstellungsseite](https://sgitaize.github.io/dubv4cgm/config/) (Deutsch / Englisch) |

| Im Ziel | Veralteter Wert | Niedrig, Thema „91 Dub v2.0“ |
|---|---|---|
| ![](docs/screenshots/cgm-flat.png) | ![](docs/screenshots/cgm-stale.png) | ![](docs/screenshots/cgm-low-theme.png) |

## Funktionen

- **CGM** in der oberen Zeile des Panels: Wert, Trendpfeil, Delta
  - **zwei Datenquellen:** Nightscout (`/pebble`, Standard) oder [iPump-mini](https://github.com/sgitaize/iPump-mini), die iPhone-App, die eine MiniMed 780G direkt ausliest – ganz ohne Internet; Nightscout kann als Ersatz einspringen (kleine Wolke)
  - hohe / niedrige Werte in eigenen Farben, veraltete Werte durchgestrichen, Hinweis **APP ÖFFNEN**, wenn iPump-mini nicht läuft
  - auf Wunsch Vibration bei niedrig / hoch (höchstens alle 10 min)
  - auf Wunsch: **Beleuchtung leuchtet in der Farbe für hoch / niedrig** (RGB-Beleuchtung der Pebble Time 2)
  - mg/dL oder mmol/L, Sensor-Intervall 5 min oder 1 min
  - akkuschonender Abruf direkt nach jedem neuen Wert, Wächter, wenn das Handy still bleibt; der letzte Wert übersteht einen Neustart
- **Vier Randbeschriftungen** (LIGHT / PREV / NEXT / unten links) zeigen wahlweise: Original-Beschriftung, CGM-Wert, CGM-Alter, Wetter, Schritte, Puls, Akku
- **Wetter** von [Open-Meteo](https://open-meteo.com) (ohne API-Schlüssel), °C / °F
- **Deutsch / Englisch** für alle Texte auf der Uhr und die Einstellungsseite
- Alles aus 91 Dub v5 plus: Schritte / Schlaf, Puls, 3 Digitalschriften, Kantenglättung, 2 Farbsätze (Wechsel nach Uhrzeit oder Antippen), über 200 Farbvorlagen, eigene Beleuchtungsfarbe, Stromsparen, Linkshänder-Modus …

## Architektur

<p align="center"><img src="docs/architecture.png" alt="Architektur: Watchface und C-Teil von pebble-cgm auf der Uhr sprechen per AppMessage mit PebbleKit JS auf dem Handy, das Nightscout oder iPump-mini und Open-Meteo abfragt; Einstellungen kommen von einer Seite auf GitHub Pages" width="100%"></p>

<sub>Quelle: <a href="docs/architecture.mmd">docs/architecture.mmd</a> (Mermaid)</sub>

**So funktioniert es**
- **Die Uhr zeichnet, das Handy holt die Daten.** Der C-Code zeichnet Watchface, Komplikationen und Beleuchtungsfarbe; das JavaScript in der Pebble-App (PebbleKit JS) spricht mit dem Internet und schickt kompakte AppMessages.
- **pebble-cgm** übernimmt alles rund um CGM auf beiden Seiten: Quelle wählen, Abruf passend nach jedem neuen Wert, Weckplan von iPump-mini, Ersatz durch Nightscout, Sendewarteschlange mit Bestätigung und ein Wächter auf der Uhr. Dieselbe Bibliothek läuft in [CasioCGM](https://github.com/sgitaize/casiocgm) und [Nightscout-supercgm](https://github.com/sgitaize/Nightscout-supercgm).
- **Die Einstellungen** sind eine statische Seite auf GitHub Pages: oben die Schnelleinrichtung (Sprache, Farbvorlage, CGM-Quelle, Komplikationen), alles andere aufklappbar. Die Konfiguration reist im URL-Fragment, dein Nightscout-Token erreicht also nie einen Server.

| Baustein | Ort | Aufgabe |
|---|---|---|
| Watchface | `src/c/main.c`, `src/c/complications.c` | Zeichnen, Randbeschriftungen, CGM-Zeile, Beleuchtungsfarbe |
| CGM (Uhr) | `src/c/cgm.c` | letzter Wert, Status, Wächter, Weck-Timer (aus pebble-cgm) |
| Handy-Skript | `src/pkjs/index.js` | Einstellungen, Wetter, Nachrichten an die Uhr |
| CGM (Handy) | `src/pkjs/cgm.js` | Nightscout / iPump-mini, Zeitplan, Ersatzquelle (aus pebble-cgm) |
| Einstellungsseite | `docs/config/` | Schnelleinrichtung, über 200 Vorlagen, Deutsch / Englisch |
| Store-Eintrag | `store/` | Beschreibung, Screenshots, `publish.sh` |
| Entwickler-Notizen | [`AGENTS.md`](AGENTS.md) | Nachrichten-Schlüssel, Einstellungen, Stolperfallen |

## Einstellungen

**CGM:** Nightscout oder iPump-mini wählen. Für Nightscout die Adresse deiner Seite eintragen (z. B. `https://my-site.herokuapp.com`) und, falls nötig, ein Token mit Leserechten – das Watchface nutzt den Endpunkt `/pebble`. Bei iPump-mini lässt sich Nightscout als Ersatz einschalten.

## Selbst bauen

```bash
npm install
pebble build          # nach Änderungen an messageKeys vorher pebble clean
pebble install --emulator emery
```

## Status & Pläne

- **Live:** 1.2.1 im Pebble Appstore (Nightscout und iPump-mini, Stabilitäts-Fixes aus pebble-cgm 1.1.0).
- Rückmeldungen gerne über die [Seite im Pebble Appstore](https://apps.rePebble.com/49c4f560ff4141548025f29f).

## Dank

Ein Fork von [91 Dub v5 plus](https://codeberg.org/dflo/91-dub-v5-plus) von dflo (selbst ein Fork von [91 Dub v5](https://codeberg.org/lightrush/91-dub-v5) von lightrush), aufbauend auf dem großartigen [91 Dub v4.0](https://github.com/orviwan/91-dub-4.0) von Orviwan – danke! Die CGM-Ergänzungen sind zu rund 95 % mit [Claude Code](https://claude.com/claude-code) entstanden.

## Lizenz

MIT-artig, siehe [LICENSE](LICENSE). Die Schriften behalten ihre eigenen Lizenzen.

Ein Hobbyprojekt, **kein Medizinprodukt**. Bitte keine Therapieentscheidungen auf Grundlage dieses Watchfaces treffen.
