import subprocess
from sentence_transformers import SentenceTransformer


# ==========================================
# 1. THE REAL AI EMBEDDING SERVICE
# ==========================================
class TextEmbedder:
    def __init__(self):
        print("🧠 Loading HuggingFace AI Model (all-MiniLM-L6-v2)...")
        # This is a real, lightning-fast 384-dimensional AI translator!
        self.model = SentenceTransformer('all-MiniLM-L6-v2')

    def embed(self, text: str) -> list[float]:
        # Translates the English text into 384 floats
        return self.model.encode(text).tolist()


# ==========================================
# 2. THE DATABASE DRIVER
# ==========================================
class NanoVectorDriver:
    def __init__(self, executable_path):
        self.exe_path = executable_path

    def execute(self, commands: list[str]) -> str:
        full_input = "\n".join(commands) + "\nEXIT\n"
        result = subprocess.run(
            [self.exe_path], input=full_input, text=True, capture_output=True
        )
        return result.stdout

    def parse_search_result(self, raw_output: str) -> tuple[str, float]:
        lines = raw_output.split('\n')
        for line in lines:
            if "1." in line and "(Distance:" in line:
                doc_id = line.split(".")[1].split("(Distance:")[0].strip()
                dist_str = line.split("(Distance:")[1].replace(")", "").strip()
                return doc_id, float(dist_str)
        return "No match", 999.9


# ==========================================
# 3. THE RAG APPLICATION (With Sexy Data)
# ==========================================
class RAGApplication:
    def __init__(self, db_path):
        self.db = NanoVectorDriver(db_path)
        self.embedder = TextEmbedder()
        self.knowledge_base = {}

        # Real AI vectors have different distance scales. 1.2 is a good strict threshold here.
        self.distance_threshold = 1.35

    def add_document(self, doc_id: str, content: str):
        self.knowledge_base[doc_id] = content
        vector = self.embedder.embed(content)

        # We now dynamically create a massive string of 384 floats for the C++ CLI!
        vector_str = " ".join([str(round(v, 6)) for v in vector])
        cmd = f"INSERT {doc_id} {vector_str}"
        self.db.execute([cmd])

    def ask_question(self, question: str):
        query_vector = self.embedder.embed(question)
        vector_str = " ".join([str(round(v, 6)) for v in query_vector])

        cmd = f"SEARCH 1 {vector_str}"
        raw_output = self.db.execute([cmd])

        best_doc_id, distance = self.db.parse_search_result(raw_output)

        if distance > self.distance_threshold:
            print(f"\n[AI Assistant] (Rejected - Distance: {distance})")
            print(
                "> \"I don't have a clever response for that yet. Ask me about C++, Python, or why I should be hired.\"")
            return

        retrieved_context = self.knowledge_base.get(best_doc_id, "Context missing.")
        print(f"\n[AI Assistant] (Distance: {distance}):")
        print(f"> \"{retrieved_context}\"")


if __name__ == "__main__":
    # Ensure this matches your exact CLion build path!
    DB_PATH = "/Users/arnabdas/Desktop/nanovector project/cmake-build-debug/nanovector_project"

    app = RAGApplication(DB_PATH)

    print("⏳ Initializing The Sarcastic Tech-Bro Knowledge Base...")
    app.add_document("doc_mom",
                     "My mom is doing great—thick, sexy, and structurally sound. Kind of like my C++ backend architecture.")
    app.add_document("doc_harddrive",
                     "I named my hard drive 'dat ass,' so once a month my computer asks if I want to 'back dat ass up.'")
    app.add_document("doc_dating",
                     "I don't need dating apps. I already have a highly optimized KD-Tree that finds my nearest neighbors in O(log N) time.")
    app.add_document("doc_bug",
                     "How do I fix bugs? I don't. I just glare at the compiler until it gets intimidated and fixes itself.")
    app.add_document("doc_sleep",
                     "Sleep? Sleep is a blocking operation. I operate strictly on O(1) caffeine loops and the tears of Python developers.")
    app.add_document("doc_roll",
                     "Not thicker than OOTKAL ROII's chicken roll.")
    print("✅ System Ready. Try asking: 'Why should we hire you?', 'Thoughts on Python?', or 'How do you fix bugs?'")

    while True:
        user_input = input("\n👤 You: ")
        if user_input.lower() in ['quit', 'exit']:
            print("Shutting down RAG system. Goodbye!")
            break

        app.ask_question(user_input)