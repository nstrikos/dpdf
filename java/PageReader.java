package dpdf;

import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.pdmodel.PDPage;
import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDDocumentOutline;
import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDOutlineItem;

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
				getOutlines(pageReader.document);
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

	public static void getOutlines(PDDocument document) throws IOException {
		PDDocumentOutline outline = document.getDocumentCatalog().getDocumentOutline();
		PDOutlineItem item = outline.getFirstChild();
		PDPage currentPage;
		Integer pageNumber;
		while( item != null )
		{
			currentPage = item.findDestinationPage(document);
			if (currentPage != null) {
				pageNumber = document.getDocumentCatalog().getPages().indexOf(currentPage) + 1;
				System.out.println(item.getTitle() + "@pageReader@" + pageNumber + "@pageReader end page@");
			}
			PDOutlineItem child = item.getFirstChild();
			while( child != null )
			{
				currentPage = child.findDestinationPage(document);
				if (currentPage != null) {
					pageNumber = document.getDocumentCatalog().getPages().indexOf(currentPage) + 1;
					System.out.println(child.getTitle() + "@pageReader@" + pageNumber + "@pageReader end page@");
				}
				child = child.getNextSibling();
			}
			item = item.getNextSibling();
		}
		System.out.println("@pageReader finished contents@");
	}
}
