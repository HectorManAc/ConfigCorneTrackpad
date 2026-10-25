#!/usr/bin/env bash
# Flashea un .hex (por defecto corne_trackpad.hex) en un Pro Micro (ATmega32U4, bootloader Caterina).
# Uso: [FLASH_HEX=archivo.hex] [FLASH_TIMEOUT=seg] ./flash.sh [IZQUIERDA|DERECHA]
set -u

HEX="$(cd "$(dirname "$0")" && pwd)/${FLASH_HEX:-corne_trackpad.hex}"  # otro archivo: FLASH_HEX=archivo.hex
LABEL="${1:-}"
TIMEOUT="${FLASH_TIMEOUT:-20}"  # segundos; se puede ampliar con FLASH_TIMEOUT=60

if ! command -v avrdude >/dev/null 2>&1; then
  echo "ERROR: avrdude no está instalado (brew install avrdude)." >&2
  exit 1
fi
if [[ ! -f "$HEX" ]]; then
  echo "ERROR: no se encuentra $HEX" >&2
  exit 1
fi

list_ports() { ls /dev/cu.usbmodem* 2>/dev/null | sort; }

BEFORE="$(list_ports)"
echo "Puertos actuales: ${BEFORE:-(ninguno)}"
echo
echo ">>> Presiona DOS VECES rápido el botón de reset de la mitad ${LABEL}..."
echo "    (esperando hasta ${TIMEOUT} s a que aparezca el bootloader)"

PORT=""
end=$(( $(date +%s) + TIMEOUT ))
while (( $(date +%s) < end )); do
  NOW="$(list_ports)"
  PORT="$(comm -13 <(echo "$BEFORE") <(echo "$NOW") | grep -m1 . || true)"
  [[ -n "$PORT" ]] && break
  sleep 0.2
done

if [[ -z "$PORT" ]]; then
  echo "ERROR: no apareció ningún /dev/cu.usbmodem* nuevo en ${TIMEOUT} s." >&2
  exit 2
fi

echo "Bootloader detectado en $PORT — flasheando..."
sleep 0.5  # deja que el puerto termine de enumerar
avrdude -p atmega32u4 -c avr109 -P "$PORT" -U "flash:w:${HEX}:i"
rc=$?
if (( rc == 0 )); then
  echo "OK: mitad ${LABEL} flasheada."
else
  echo "ERROR: avrdude terminó con código $rc." >&2
fi
exit $rc
