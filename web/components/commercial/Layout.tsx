import { Craft } from "./Craft";
import { Footer } from "./Footer";
import { Hero } from "./Hero";
import { Pricing } from "./Pricing";
import { Showcase } from "./Showcase";
import { Specs } from "./Specs";

export function CommercialLayout() {
  return (
    <>
      <Hero />
      <Showcase />
      <Craft />
      <Pricing />
      <Specs />
      <Footer />
    </>
  );
}
