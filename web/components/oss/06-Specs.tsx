import { specs } from "@/content/features";
import { FaApple, FaLinux, FaWindows } from "react-icons/fa6";

export function Specs() {
  const renderValue = (label: string, value: string) => {
    if (label !== "Platforms") {
      return value;
    }

    return (
      <span className="flex flex-wrap items-center gap-4">
        <span className="inline-flex items-center gap-1.5">
          <FaWindows className="text-[0.95rem]" />
          Windows 10+
        </span>
        <span className="inline-flex items-center gap-1.5">
          <FaApple className="text-[0.95rem]" />
          macOS 12+
        </span>
        <span className="inline-flex items-center gap-1.5">
          <FaLinux className="text-[0.95rem]" />
          Linux
        </span>
      </span>
    );
  };

  return (
    <section id="specs" className="relative px-6 py-24 md:py-32">
      <div className="mx-auto max-w-6xl">
        <div
          className="overflow-hidden border bg-bg-2"
          style={{ borderColor: "var(--color-hairline)", borderRadius: "4px" }}
        >
          <div
            className="flex items-center justify-between border-b px-5 py-3 font-mono-accent text-[11px]"
            style={{
              borderColor: "var(--color-hairline)",
              background: "var(--color-bg-3)",
              color: "var(--color-ink-soft)",
            }}
          >
            <span>SPECS.md</span>
            <span>{specs.length} entries</span>
          </div>

          <table className="w-full font-mono-accent text-sm">
            <tbody>
              {specs.map((s, i) => (
                <tr
                  key={s.label}
                  className="border-t"
                  style={{ borderColor: "var(--color-hairline)" }}
                >
                  <td
                    className="w-8 px-4 py-4 text-right text-[11px]"
                    style={{ color: "var(--color-ink-dim)" }}
                  >
                    {String(i + 1).padStart(2, "0")}
                  </td>
                  <td
                    className="py-4 pr-4 uppercase tracking-[0.18em] text-[11px]"
                    style={{ color: "var(--color-ink-soft)" }}
                  >
                    {s.label}
                  </td>
                  <td
                    className="py-4 pr-6"
                    style={{
                      fontFamily: "var(--font-space-grotesk)",
                      fontWeight: 500,
                      fontSize: "0.95rem",
                      color: "var(--color-ink)",
                    }}
                  >
                    {renderValue(s.label, s.value)}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    </section>
  );
}
