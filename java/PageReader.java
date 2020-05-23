package dpdf;

import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.pdmodel.PDPage;
import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDDocumentOutline;
import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDOutlineItem;
import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDOutlineNode;

import org.apache.pdfbox.text.PDFTextStripper;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

public class PageReader {

	public PDDocument document;
	public PDFTextStripper reader;

	public static void main(String[] args) throws Exception{

		PageReader pageReader = new PageReader();

		String input = "";

		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in)); 

		while (input != "quit") {

			input = reader.readLine();

			if (input.equals("quit"))
				break;

			if (input.startsWith("open")) {
				String filename = input.substring(5, input.length());
				if (pageReader.document != null)
					pageReader.document.close();

				try {
					pageReader.document = PDDocument.load(new File(filename));
					pageReader.reader = new PDFTextStripper();

					System.out.print(pageReader.document.getNumberOfPages());
					System.out.println("@pageReader number of pages@");
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

			if (input.startsWith("page")) {
				String page = input.substring(5, input.length());
				int pageid = Integer.parseInt(page);
				pageReader.pageText(pageid);
			}

			if (input.equals("contents")) {
				PDDocumentOutline outline = pageReader.document.getDocumentCatalog().getDocumentOutline();
				getOutlines(pageReader.document, outline, "");
				System.out.println("@pageReader finished contents@");
			}
		}

		if (pageReader.document != null)
			pageReader.document.close();
	}

	void pageText (int page) {
		try {
			reader.setStartPage(page);
			reader.setEndPage(page);
			String pageText = reader.getText(document);
			System.out.println(pageText);
			System.out.println("@pageReader end of page@");
		} catch (IOException e) {
			e.printStackTrace();
		}		
	}

	public static void getOutlines(PDDocument document, PDOutlineNode bookmark, String indentation) throws IOException {
		if (bookmark == null)
			return;
		PDOutlineItem current = bookmark.getFirstChild();
		while (current != null) {
			PDPage currentPage = current.findDestinationPage(document);
			if (currentPage != null) {
				Integer pageNumber = document.getDocumentCatalog().getPages().indexOf(currentPage) + 1;
				System.out.println(current.getTitle() + "@pageReader@" + pageNumber + "@pageReader end page@");
			}
			getOutlines(document, current, indentation);
			current = current.getNextSibling();
		}
	}
}
