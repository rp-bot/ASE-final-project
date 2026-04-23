export type DonationTier = {
  name: string;
  amount: string;
  tagline: string;
  perks: string[];
  platform: "kofi" | "sponsors" | "bmac";
};

export const donationTiers: DonationTier[] = [
  {
    name: "Coffee",
    amount: "$5",
    tagline: "One-shot thanks.",
    perks: ["Warm feelings", "Fuels a build night"],
    platform: "bmac",
  },
  {
    name: "Patron",
    amount: "$15 / mo",
    tagline: "Keep the synth humming.",
    perks: ["Name in CONTRIBUTORS.md", "Early-access builds"],
    platform: "sponsors",
  },
  {
    name: "Supporter",
    amount: "$50 / mo",
    tagline: "Commit to the cube.",
    perks: [
      "Everything in Patron",
      "Vote on roadmap priorities",
      "Discord supporter role",
    ],
    platform: "kofi",
  },
];

export const donationLinks = {
  sponsors: "https://github.com/sponsors/",
  kofi: "https://ko-fi.com/",
  bmac: "https://buymeacoffee.com/",
};

export const ossCopy = {
  heroBadge: "v1.0 — Open Source · GPLv3",
  heroSubtitle:
    "The full synthesizer, source and all, under GPLv3. No tiers. No trials. Free forever — and funded by the people who play through it.",
  heroPrimary: { label: "Download free", href: "#download" },
  heroSecondary: { label: "Star on GitHub", href: "#github" },
  licenseNote: "GPLv3 · Source on GitHub · Donations optional but loved",
  footerNote: "Made with open source, paid for with coffee.",
  repoUrl: "https://github.com/rp-bot/ASE-final-project",
  
};

export const downloadPlatforms = [
  { os: "Windows", formats: ["VST3", "AAX"], href: "#dl-win" },
  { os: "macOS", formats: ["VST3", "AU", "AAX"], href: "#dl-mac" },
  { os: "Linux", formats: ["VST3"], href: "#dl-linux" },
];
