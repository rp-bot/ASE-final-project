import type { Metadata } from "next";
import {
  Bebas_Neue,
  Cormorant_Garamond,
  DM_Sans,
  IBM_Plex_Sans,
  Fraunces,
  Inter,
  JetBrains_Mono,
  Manrope,
  Space_Grotesk,
} from "next/font/google";
import { EditionProvider } from "@/lib/edition";
import "./globals.css";

const inter = Inter({
  subsets: ["latin"],
  variable: "--font-inter",
  display: "swap",
});

const fraunces = Fraunces({
  subsets: ["latin"],
  variable: "--font-fraunces",
  display: "swap",
  axes: ["opsz", "SOFT"],
});

const spaceGrotesk = Space_Grotesk({
  subsets: ["latin"],
  variable: "--font-space-grotesk",
  display: "swap",
});

const jetbrains = JetBrains_Mono({
  subsets: ["latin"],
  variable: "--font-jetbrains",
  display: "swap",
});

const bebas = Bebas_Neue({
  subsets: ["latin"],
  variable: "--font-bebas",
  display: "swap",
  weight: "400",
});

const cormorant = Cormorant_Garamond({
  subsets: ["latin"],
  variable: "--font-cormorant",
  display: "swap",
});

const dmSans = DM_Sans({
  subsets: ["latin"],
  variable: "--font-dm-sans",
  display: "swap",
});

const ibmPlexSans = IBM_Plex_Sans({
  subsets: ["latin"],
  variable: "--font-ibm-plex-sans",
  display: "swap",
  weight: ["400", "500", "600"],
});

const manrope = Manrope({
  subsets: ["latin"],
  variable: "--font-manrope",
  display: "swap",
});

export const metadata: Metadata = {
  title: "Volumetric Vector Synthesis — Fly through a cube of sound",
  description:
    "An 8-source 3D vector synthesizer. Blend eight timbres through a cubic space with trilinear interpolation and a GPU-rendered glass-cube interface.",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html
      lang="en"
      data-edition="commercial"
      data-studio-theme="studio-3"
      className={`${inter.variable} ${fraunces.variable} ${spaceGrotesk.variable} ${jetbrains.variable} ${bebas.variable} ${cormorant.variable} ${dmSans.variable} ${ibmPlexSans.variable} ${manrope.variable}`}
    >
      <body>
        <EditionProvider>{children}</EditionProvider>
      </body>
    </html>
  );
}
