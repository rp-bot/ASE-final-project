import { Downloads } from "./Downloads";
import { Footer } from "./Footer";
import { Hero } from "./Hero";
import { Internals } from "./Internals";
import { Readme } from "./Readme";
import { Showcase } from "./Showcase";
import { Specs } from "./Specs";

export function OssLayout() {
  return (
    <>
      <Hero />
      <Readme />
      <Showcase />
      <Internals />
      <Downloads />
      <Specs />
      <Footer />
    </>
  );
}
