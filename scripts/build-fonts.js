#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const PROJECT_ROOT = __dirname + '/..';
const FONTS_DIR = path.join(PROJECT_ROOT, 'resources', 'fonts');
const RES_DIR = path.join(PROJECT_ROOT, 'resources');
const BUILD_DIR = path.join(PROJECT_ROOT, 'build', 'fonts');

function log(msg) {
  if (msg !== undefined) {
    console.log(msg);
  }
}

function error(msg) {
  console.error(msg);
}

function checkTool(name, checkPath) {
  try {
    if (checkPath) {
      fs.accessSync(checkPath, fs.constants.X_OK);
    } else {
      const result = spawnSync('which', [name], { stdio: 'pipe' });
      if (result.status !== 0) return false;
    }
    return true;
  } catch (e) {
    return false;
  }
}

function checkTools() {
  const tools = [
    { name: 'pyftsubset', path: null },
    { name: 'fontforge', path: null },
    { name: 'fctx-compiler', path: path.join(PROJECT_ROOT, 'node_modules', '.bin', 'fctx-compiler') }
  ];

  const missing = [];
  for (const tool of tools) {
    if (!checkTool(tool.name, tool.path)) {
      missing.push(tool.name);
    }
  }

  if (missing.length > 0) {
    error('Missing required tool(s): ' + missing.join(', '));
    error('Please install the missing tool(s) and try again.');
    process.exit(1);
  }
}

function expandCharRegex(regexStr) {
  const chars = new Set();
  const inner = regexStr.replace(/^\[|\]$/g, '');

  for (let i = 0; i < inner.length; i++) {
    const ch = inner[i];
    if (ch === '-' && i > 0 && i < inner.length - 1) {
      const start = inner[i - 1];
      const end = inner[i + 1];
      for (let c = start.charCodeAt(0); c <= end.charCodeAt(0); c++) {
        chars.add(String.fromCharCode(c));
      }
    } else if (ch === '\\' && i + 1 < inner.length) {
      chars.add(inner[i + 1]);
      i++;
    } else {
      chars.add(ch);
    }
  }

  return Array.from(chars).join('');
}

function getFileSize(filePath) {
  try {
    return fs.statSync(filePath).size;
  } catch (e) {
    return null;
  }
}

function cleanupBuildDir() {
  try {
    fs.rmSync(BUILD_DIR, { recursive: true, force: true });
  } catch (e) {
    // ignore cleanup errors
  }
}

function main() {
  process.chdir(PROJECT_ROOT);

  log('Checking for required tools...');
  checkTools();
  log('All tools found.\n');

  // Read package.json
  const pkgPath = path.join(PROJECT_ROOT, 'package.json');
  const pkg = JSON.parse(fs.readFileSync(pkgPath, 'utf8'));
  const mediaEntries = pkg.pebble.resources.media || [];

  // Collect characterRegex patterns per TTF file from type: "font" entries
  const regexByTtf = {};
  for (const entry of mediaEntries) {
    if (entry.type !== 'font' || !entry.characterRegex) continue;
    if (!entry.file.match(/\.ttf$/i)) continue;
    if (!regexByTtf[entry.file]) {
      regexByTtf[entry.file] = new Set();
    }
    regexByTtf[entry.file].add(entry.characterRegex);
  }

  // Build plan from type: "raw" .ffont entries
  const plan = [];
  for (const entry of mediaEntries) {
    if (entry.type !== 'raw') continue;
    if (!entry.file.match(/\.ffont$/i)) continue;

    const ffontFile = entry.file;
    const ttfFile = ffontFile.replace(/\.ffont$/i, '.ttf');
    const ttfBase = path.basename(ttfFile, '.ttf');

    // Aggregate characterRegex patterns for this TTF
    const allChars = new Set();
    const regexes = regexByTtf[ttfFile];
    if (regexes) {
      for (const regex of regexes) {
        const expanded = expandCharRegex(regex);
        for (const ch of expanded) {
          allChars.add(ch);
        }
      }
    }

    plan.push({
      ttfBase: ttfBase,
      ttfFile: ttfFile,
      ttfFullPath: path.join(RES_DIR, ttfFile),
      ffontFile: ffontFile,
      ffontName: path.basename(ffontFile),
      ffontPath: path.join(FONTS_DIR, path.basename(ffontFile)),
      textStr: Array.from(allChars).join(''),
      charCount: allChars.size
    });
  }

  if (plan.length === 0) {
    log('No ffont definitions found in package.json.');
    return;
  }

  // Output plan
  log('Processing plan:');
  for (const item of plan) {
    log(`  ${item.ttfFile} -> ${item.ffontFile} (${item.charCount} chars)`);
  }
  console.log();

  // Create build directory
  fs.mkdirSync(BUILD_DIR, { recursive: true });

  // Execute plan
  const results = [];
  for (const item of plan) {
    const oldSize = getFileSize(item.ffontPath);
    log(`Processing ${item.ttfBase}...`);

    const subsetTtf = path.join(BUILD_DIR, `${item.ttfBase}_subset.ttf`);
    const subsetSvg = path.join(BUILD_DIR, `${item.ttfBase}_subset.svg`);

    // Step 1: Subset TTF
    const result1 = spawnSync('pyftsubset', [
      item.ttfFullPath,
      '--text=' + item.textStr,
      '--output-file=' + subsetTtf,
      '--ignore-missing-glyphs'
    ], { stdio: 'pipe' });
    if (result1.status !== 0) {
      error(`  pyftsubset failed for ${item.ttfBase}: ${result1.stderr.toString().trim()}`);
      results.push({ name: item.ffontName, status: 'FAIL', oldSize: oldSize, newSize: null });
      continue;
    }
    const origSize = getFileSize(item.ttfFullPath);
    const subsetSize = getFileSize(subsetTtf);
    log(`  Subset: ${origSize} -> ${subsetSize} bytes (${Math.round((1 - subsetSize / origSize) * 100)}% reduction)`);

    // Step 2: Convert to SVG
    const script = `import fontforge; fontforge.open('${subsetTtf}').generate('${subsetSvg}')`;
    const result2 = spawnSync('fontforge', ['-lang=py', '-c', script], { stdio: 'pipe' });
    if (result2.status !== 0) {
      error(`  fontforge failed for ${item.ttfBase}: ${result2.stderr.toString().trim()}`);
      results.push({ name: item.ffontName, status: 'FAIL', oldSize: oldSize, newSize: null });
      continue;
    }

    // Step 3: Compile to ffont
    const compilerPath = path.join(PROJECT_ROOT, 'node_modules', '.bin', 'fctx-compiler');
    const result3 = spawnSync(compilerPath, [subsetSvg], { stdio: 'inherit' });
    if (result3.status !== 0) {
      error(`  fctx-compiler failed for ${item.ttfBase}`);
      results.push({ name: item.ffontName, status: 'FAIL', oldSize: oldSize, newSize: null });
      continue;
    }

    // Move output from resources/ to resources/fonts/
    const generatedFfont = path.join(RES_DIR, item.ffontName);
    if (fs.existsSync(generatedFfont)) {
      fs.copyFileSync(generatedFfont, item.ffontPath);
      fs.unlinkSync(generatedFfont);
    } else {
      error(`  Generated .ffont not found at ${generatedFfont}`);
      results.push({ name: item.ffontName, status: 'FAIL', oldSize: oldSize, newSize: null });
      continue;
    }

    const newSize = getFileSize(item.ffontPath);
    const reduction = origSize ? Math.round((1 - newSize / origSize) * 100) : null;
    results.push({ name: item.ffontName, status: 'OK', oldSize: origSize, newSize: newSize, reduction: reduction });
    log(`  Output: ${item.ffontName} (${newSize} bytes)`);
    console.log();
  }

  // Cleanup build directory
  cleanupBuildDir();

  // Summary
  log('Summary:');
  for (const r of results) {
    if (r.status === 'OK') {
      log(`  ${r.name}: ${r.oldSize} -> ${r.newSize} bytes (${r.reduction}% reduction) [OK]`);
    } else {
      log(`  ${r.name}: [FAIL]`);
    }
  }
}

main();
