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

	public static void main(String[] args) throws Exception {

		PageReader pageReader = new PageReader();

		String userInput = "";

		//bufferReader reads console input
		BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in)); 

		while (!userInput.equals("quit")) {

			//read user input
			userInput = bufferedReader.readLine();

			if (userInput.startsWith("open")) {
				
				//read filename from user input
				String filename = userInput.substring(5, userInput.length());
				pageReader.openFile(filename);
			}

			if (userInput.startsWith("page")) {
				
				//read page from user input
				String page = userInput.substring(5, userInput.length());
				int pageid = Integer.parseInt(page);
				pageReader.pageText(pageid);
			}

			if (userInput.equals("contents")) {
				PDDocumentOutline outline = pageReader.document.getDocumentCatalog().getDocumentOutline();
				pageReader.getOutlines(pageReader.document, outline);
				System.out.println("@pageReader finished contents@");
			}
		}

		if (pageReader.document != null)
			pageReader.document.close();
	}

	void openFile(String filename) {
		try {			
			if (document != null)
				document.close();
			document = PDDocument.load(new File(filename));
			reader = new PDFTextStripper();

			System.out.print(document.getNumberOfPages());
			System.out.println("@pageReader number of pages@");
		} catch (IOException e) {
			e.printStackTrace();
		}	
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

	void getOutlines(PDDocument document, PDOutlineNode bookmark) throws IOException {
		if (bookmark == null)
			return;
		PDOutlineItem current = bookmark.getFirstChild();
		while (current != null) {
			PDPage currentPage = current.findDestinationPage(document);
			if (currentPage != null) {
				Integer pageNumber = document.getDocumentCatalog().getPages().indexOf(currentPage) + 1;
				System.out.println(current.getTitle() + "@pageReader@" + pageNumber + "@pageReader end page@");
			}
			getOutlines(document, current);
			current = current.getNextSibling();
		}
	}
}
