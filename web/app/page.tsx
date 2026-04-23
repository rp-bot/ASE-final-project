import { EditionSwitcher } from "@/components/EditionSwitcher";
import { Nav } from "@/components/Nav";

export default function Home() {
  return (
    <main className="relative min-h-screen">
      <Nav />
      <EditionSwitcher />
    </main>
  );
}
