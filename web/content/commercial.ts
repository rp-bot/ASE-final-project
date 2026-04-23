export type CommercialTier = {
  name: string;
  price: number;
  tagline: string;
  includes: string[];
  cta: string;
  featured?: boolean;
};

export const tiers: CommercialTier[] = [
  {
    name: "Standard",
    price: 99,
    tagline: "One license. One workstation. Forever.",
    includes: [
      "Personal-use license",
      "All VST3, AU, AAX builds",
      "Free minor updates for life",
      "Factory preset library",
      "Email support",
    ],
    cta: "Buy Standard — $99",
  },
  {
    name: "Studio",
    price: 179,
    tagline: "Commercial rights, three seats.",
    includes: [
      "Commercial-use license",
      "Three concurrent installs",
      "Free minor + major updates for life",
      "Factory + expansion preset packs",
      "Priority email support",
      "Beta program access",
    ],
    cta: "Buy Studio — $179",
    featured: true,
  },
];

export const commercialCopy = {
  heroBadge: "v1.0 — Commercial Edition",
  heroSubtitle:
    "A volumetric instrument for modern sound design: 8 sources in a 3D cube, smoothly blended as you move. One-time license with lifetime minor updates.",
  heroPrimary: { label: "Buy license", href: "#pricing" },
  heroSecondary: { label: "Download 14-day trial", href: "#trial" },
  licenseNote:
    "One-time purchase · No subscription · 30-day refund · VAT invoiced",
  footerNote: "© Volumetric Vector Synthesis. All rights reserved.",
};
