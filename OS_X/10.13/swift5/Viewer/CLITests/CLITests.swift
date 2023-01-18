//
//  CLITests.swift
//  CLITests
//
//  Created by Jérôme LAURENS on 02/01/2023.
//  Copyright © 2023 Jérôme LAURENS. All rights reserved.
//

import XCTest
import synctex

class CLITests: XCTestCase {
    let testFilesDirectoryURL = URL(fileURLWithPath: #file).deletingLastPathComponent().deletingLastPathComponent().deletingLastPathComponent().deletingLastPathComponent().deletingLastPathComponent().deletingLastPathComponent().appendingPathComponent("synctex test files", isDirectory: true).absoluteURL
    override func setUp() {
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }

    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }
    
    func directoryURL(_ d: String)-> URL {
        return URL(fileURLWithPath: d, relativeTo: testFilesDirectoryURL)
    }
    
    func testExample() {
        print("********** testFilesDirectoryURL: \(testFilesDirectoryURL)")
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct results.
    }
    func withArrayOfCStrings<R>(_ args: [String],
                                       _ body: (UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> R) -> R {
        var cStrings = args.map { strdup($0) }
        cStrings.append(nil)
        defer {
            cStrings.forEach { free($0) }
        }
        return body(&cStrings)
    }
    func test_all_nodes_hboxes() {
        let path = directoryURL("1/all nodes hboxes").appendingPathComponent("1.pdf").path
        let arguments = [
            "-o",
            "1:0:0:"+path
        ]
        let argc = Int32(arguments.count)
        return withArrayOfCStrings(arguments) {
            argv in
            synctex_edit(argc,argv, synctex_print)
            print("******** argc", argc)
            print("******** \(arguments[1])")
        }
    }
    func generic_test_1( _ f: (synctex_scanner_p)->Int32 ) {
        let path = directoryURL("work/base.synctex").path
        let output = strdup(path)
        let build_directory = strdup("")
        if let scanner = synctex_scanner_new_with_output_file(output, build_directory, 1) {
            XCTAssert(f(scanner)==0)
        } else {
            XCTFail("No scanner")
        }
        free(output)
        free(build_directory)
    }
    func generic_test_2( _ f: (synctex_scanner_p)->Int32, file: String ) {
        let path = directoryURL("work/\(file).synctex").path
        let output = strdup(path)
        let build_directory = strdup("")
        if let scanner = synctex_scanner_new_with_output_file(output, build_directory, 1) {
            XCTAssert(f(scanner)==0)
        } else {
            XCTFail("No scanner")
        }
        free(output)
        free(build_directory)
    }
    func test_input() {
        generic_test_1(synctex_test_input)
    }
    func test_proxy() {
        generic_test_1(synctex_test_proxy)
    }
    func test_tree() {
        generic_test_1(synctex_test_tree)
    }
    func test_page() {
        generic_test_1(synctex_test_page)
    }
    func test_handle() {
        generic_test_1(synctex_test_handle)
    }
    func test_display_query() {
        generic_test_1(synctex_test_display_query)
    }
    func test_char_index() {
        XCTAssert(synctex_test_char_index()==0)
    }
    func test_sheet_1() {
        XCTAssert(synctex_test_sheet_1()==0)
    }
    func test_sheet_2() {
        XCTAssert(synctex_test_sheet_2()==0)
    }
    func test_form() {
        XCTAssert(synctex_test_form()==0)
    }
    func test_ith1() {
        let ith1 = strdup("./ith1.tex")
        generic_test_2({ (s: synctex_scanner_p) -> Int32 in
            XCTAssert(synctex_scanner_get_ith_tag(s, ith1, 1) == 333)
            XCTAssert(synctex_scanner_get_ith_tag(s, ith1, 2) == 22)
            XCTAssert(synctex_scanner_get_ith_tag(s, ith1, 3) == 1)
            return 0
        }, file: "ith1")
        free(ith1)
    }
}
