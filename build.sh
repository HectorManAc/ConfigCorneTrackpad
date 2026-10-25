#!/usr/bin/env bash
# Compila el firmware desde este repo: copia keymap/hector_tp a vial-qmk, aplica el parche local
# si falta, compila y deja el resultado en corne_trackpad.hex.
# Uso: ./build.sh            (vial-qmk en ~/vial-qmk; otra ruta: VIAL_QMK=/ruta ./build.sh)
set -euo pipefail

REPO="$(cd "$(dirname "$0")" && pwd)"
VIAL_QMK="${VIAL_QMK:-$HOME/vial-qmk}"
AVR_GCC_BIN="/opt/homebrew/opt/avr-gcc@8/bin"

if [[ ! -d "$VIAL_QMK/keyboards/crkbd" ]]; then
  echo "ERROR: no encuentro vial-qmk en $VIAL_QMK (ver README → Compilar desde cero)." >&2
  exit 1
fi
if [[ ! -x "$AVR_GCC_BIN/avr-gcc" ]]; then
  echo "ERROR: falta avr-gcc 8 (brew install osx-cross/avr/avr-gcc@8)." >&2
  exit 1
fi

# Parche local: se aplica solo si no está ya aplicado
if git -C "$VIAL_QMK" apply --check "$REPO/patches/vial-qmk-local.patch" 2>/dev/null; then
  git -C "$VIAL_QMK" apply "$REPO/patches/vial-qmk-local.patch"
  echo "Parche local aplicado a vial-qmk."
elif ! git -C "$VIAL_QMK" apply --check -R "$REPO/patches/vial-qmk-local.patch" 2>/dev/null; then
  echo "ERROR: el parche no aplica ni está aplicado; ¿cambió vial-qmk? (ver README)." >&2
  exit 1
fi

# El repo es la fuente: reemplaza el keymap de vial-qmk por el de aquí
rm -rf "$VIAL_QMK/keyboards/crkbd/keymaps/hector_tp"
cp -R "$REPO/keymap/hector_tp" "$VIAL_QMK/keyboards/crkbd/keymaps/hector_tp"

cd "$VIAL_QMK"
PATH="$VIAL_QMK/.venv/bin:$AVR_GCC_BIN:$PATH" QMK_HOME="$VIAL_QMK" make crkbd/rev1:hector_tp

cp "$VIAL_QMK/.build/crkbd_rev1_hector_tp.hex" "$REPO/corne_trackpad.hex"
echo "Listo: $REPO/corne_trackpad.hex"
