/*
 * Reversa Documentation - Helper de sidebar reativa
 *
 * Cada controle dentro da sidebar declara um data-param.
 * Mudanças disparam o evento "reversa:param-change" para
 * a página específica reagir, e persistem em localStorage.
 *
 * Botão "Reset" restaura defaults declarados em data-default.
 * Botão "Download PNG" captura o canvas principal da viewport.
 */

(function () {
    "use strict";

    const STORAGE_PREFIX = "reversa.sidebar.";

    /**
     * Inicializa todos os controles da sidebar atual.
     */
    function init() {
        const sidebar = document.querySelector(".reversa-doc-sidebar");
        if (!sidebar) return;

        const pageId = sidebar.dataset.page || document.body.dataset.page || "default";
        const controls = sidebar.querySelectorAll("[data-param]");

        controls.forEach((control) => {
            attachControl(control, pageId);
            restoreControl(control, pageId);
        });

        attachResetButton(sidebar, pageId);
        attachExportButton(sidebar);
    }

    function attachControl(control, pageId) {
        const eventName = control.type === "checkbox" ? "change" : "input";
        control.addEventListener(eventName, () => {
            const param = control.dataset.param;
            const value = readControlValue(control);
            persistValue(pageId, param, value);
            dispatchChange(param, value, control);
        });
    }

    function readControlValue(control) {
        if (control.type === "checkbox") return control.checked;
        if (control.type === "range" || control.type === "number") {
            return parseFloat(control.value);
        }
        return control.value;
    }

    function writeControlValue(control, value) {
        if (control.type === "checkbox") control.checked = !!value;
        else control.value = value;
    }

    function restoreControl(control, pageId) {
        const param = control.dataset.param;
        const key = STORAGE_PREFIX + pageId + "." + param;
        let saved;
        try {
            saved = localStorage.getItem(key);
        } catch (e) {
            return;
        }
        if (saved === null) return;

        try {
            const parsed = JSON.parse(saved);
            writeControlValue(control, parsed);
            dispatchChange(param, parsed, control);
        } catch (e) {
            /* dados corrompidos, ignora silencioso */
        }
    }

    function persistValue(pageId, param, value) {
        try {
            const key = STORAGE_PREFIX + pageId + "." + param;
            localStorage.setItem(key, JSON.stringify(value));
        } catch (e) {
            /* ignora se localStorage indisponível */
        }
    }

    function dispatchChange(param, value, control) {
        const event = new CustomEvent("reversa:param-change", {
            detail: { param, value, control },
            bubbles: true
        });
        control.dispatchEvent(event);
    }

    function attachResetButton(sidebar, pageId) {
        const btn = sidebar.querySelector("[data-action='reset']") || sidebar.querySelector("#reset");
        if (!btn) return;
        btn.addEventListener("click", () => {
            const controls = sidebar.querySelectorAll("[data-param]");
            controls.forEach((control) => {
                const defaultValue = control.dataset.default;
                if (defaultValue === undefined) return;
                let value = defaultValue;
                if (control.type === "checkbox") value = defaultValue === "true";
                else if (control.type === "range" || control.type === "number") value = parseFloat(defaultValue);
                writeControlValue(control, value);
                persistValue(pageId, control.dataset.param, value);
                dispatchChange(control.dataset.param, value, control);
            });
        });
    }

    function attachExportButton(sidebar) {
        const btn = sidebar.querySelector("[data-action='export-png']") || sidebar.querySelector("#export-png");
        if (!btn) return;
        btn.addEventListener("click", () => {
            const canvas = document.querySelector("canvas");
            if (!canvas) {
                showToast("Nenhum canvas para exportar nesta página.");
                return;
            }
            canvas.toBlob((blob) => {
                if (!blob) {
                    showToast("Falha ao gerar PNG.");
                    return;
                }
                const url = URL.createObjectURL(blob);
                const a = document.createElement("a");
                a.href = url;
                a.download = inferExportName();
                document.body.appendChild(a);
                a.click();
                document.body.removeChild(a);
                URL.revokeObjectURL(url);
            }, "image/png");
        });
    }

    function inferExportName() {
        const page = document.body.dataset.page || "view";
        const stamp = new Date().toISOString().replace(/[:.]/g, "-").slice(0, 19);
        return "reversa-" + page + "-" + stamp + ".png";
    }

    function showToast(message) {
        const existing = document.querySelector(".reversa-toast");
        if (existing) existing.remove();
        const toast = document.createElement("div");
        toast.className = "reversa-toast";
        toast.setAttribute("role", "status");
        toast.textContent = message;
        document.body.appendChild(toast);
        setTimeout(() => toast.remove(), 3500);
    }

    if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", init);
    } else {
        init();
    }
})();
