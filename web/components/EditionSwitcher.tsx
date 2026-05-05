"use client";

import { AnimatePresence, motion } from "framer-motion";
import { useEdition } from "@/lib/edition";
import { CommercialLayout } from "./commercial/00-Layout";
import { OssLayout } from "./oss/00-Layout";

export function EditionSwitcher() {
  const { edition } = useEdition();

  return (
    <AnimatePresence mode="wait" >
      <motion.div
        key={edition}
        initial={{ opacity: 0, y: 8 }}
        animate={{ opacity: 1, y: 0 }}
        exit={{ opacity: 0, y: -8 }}
        transition={{ duration: 0.45, ease: [0.22, 0.61, 0.36, 1] }}
        
      >
        {edition === "commercial" ? <CommercialLayout /> : <OssLayout />}
      </motion.div>
    </AnimatePresence>
  );
}
